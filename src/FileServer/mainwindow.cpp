#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QModelIndex>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QLabel>
#include <QSpinBox>
#include <QHostAddress>
#include <QMessageBox>
#include <QFileInfo>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->append("必须先上传文件，再开始监听！");

    m_tcpServer = new QTcpServer(this);//

    model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);

    QList<int> columnsToHide = {1, 2, 3};  // 定义一个需要隐藏的列的列表
    for (int column : columnsToHide) {  // 使用循环来隐藏列
        ui->treeView->setColumnHidden(column, true); //依次隐藏 Size、Type、Date Modified 列
    }

    ui->listView->setModel(model);

    //状态栏
    QLabel *LabSocketState=new QLabel("Socket状态：");//状态栏标签
    LabSocketState->setMinimumWidth(250);
    ui->statusbar->addWidget(LabSocketState);

    QString localIP=getLocalIP();//本机IP
    QLabel *LabIpState=new QLabel("IP地址："+localIP);//状态栏标签
    LabIpState->setMinimumWidth(250);
    ui->statusbar->addWidget(LabIpState);

    //工具栏
    ui->toolBar->addSeparator();//分隔条
    ui->toolBar->addWidget(new QLabel("端口号: "));
    spinPortEdit = new QSpinBox();
    spinPortEdit->setRange(0,65535);
    spinPortEdit->setValue(5555);

    ui->toolBar->addWidget(spinPortEdit);

    //Action栏
    ui->actDisconnect->setEnabled(false);

    // treeView中选择文件夹，listView展示文件夹中的内容
    connect(ui->treeView, &QTreeView::clicked, this, [=](const QModelIndex &index) {
        ui->listView->setRootIndex(index);
        ui->datailTextEdit->clear();//清除文本窗口
    });

    // 连接listView的clicked信号到显示详细信息的槽函数
    connect(ui->listView, &QListView::clicked, this, &MainWindow::onListViewClicked);


    // 当选中listView或者treeview中的项目，pathLineEdit 实时显示当前文件路径
    auto updatePath = [=](const QModelIndex &current) {
        // 获取当前选定项的路径
        QString path = model->filePath(current);
        ui->pathLineEdit->setText(path);
    };
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);



}

MainWindow::~MainWindow()
{
    delete ui;
}


//获取本地IP
QString MainWindow::getLocalIP()
{
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();

    if (!addList.isEmpty())
        for (int i=0;i<addList.count();i++)
        {
            QHostAddress aHost=addList.at(i);
            if (QAbstractSocket::IPv4Protocol==aHost.protocol())
            {
                localIP=aHost.toString();
                break;
            }
        }
    return localIP;
}

void MainWindow::onListViewClicked(const QModelIndex &index)
{
    // 1.可在listviw进入文件夹，同时在treeView展开点击项目的父文件夹
    QString path = model->filePath(index);    // 获取点击的项对应的路径

    if (model->isDir(index)) { // 检查点击的项是否为目录

        ui->listView->setRootIndex(index);// 进入文件夹，更新listView

        QDir dir(path);// 获取该路径对应的父文件夹路径
        dir.cdUp();
        QString parentPath = dir.absolutePath();


        QModelIndex parentIndex = model->index(parentPath);// 根据父文件夹路径找到对应的索引

        ui->treeView->expand(parentIndex);// 在treeView展开父文件夹
    }



    // 2.打印文件详细信息到文本窗口
    QFileInfo fileInfo = model->fileInfo(index);
    QString details;

    if (fileInfo.isDir()) {
        details = QString("Directory:\n %1").arg(fileInfo.filePath());
    } else {
        details = QString("File:\n %1\n" "Size:\n %2 MB(%3 bytes)\n" "Type:\n%4\n" "Last Modified:\n %5")
                      .arg(fileInfo.filePath())
                      .arg(fileInfo.size()/1048576)
                      .arg(fileInfo.size())
                      .arg(fileInfo.suffix().isEmpty() ? "Unknown" : fileInfo.suffix())
                      .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    }

    ui->datailTextEdit->setText(details);

}


bool MainWindow::checkFile(QString filePath)
{
    QFile file(filePath);
    if(!file.exists(filePath)){
        ui->textBrowser->append("上传文件不存在！");
        return false;
    }
    if(file.size()==0) {
        ui->textBrowser->append("上传文件为空文件！");
        return false;
    }
    return true;
}


void MainWindow::delNewConnect()
{
    QTcpSocket* socket = m_tcpServer->nextPendingConnection();
    ui->textBrowser->append(QString("客户端[%1]已连接！").arg(socket->peerAddress().toString()));

    connect(socket, &QTcpSocket::readyRead, [=]() {
        dealMsg(socket);
    });
}

void MainWindow::dealMsg(QTcpSocket *socket)
{

    QDataStream in(socket);
    int typeMsg;
    in>>typeMsg;
    ui->textBrowser->append(QString("收到客户端发来的消息：%1").arg(typeMsg));

    if(typeMsg == MsgType::FileInfo) {
        // 发送文件信息
        transferFileInfo(socket);
    }
    else if(typeMsg == MsgType::FileData) {
        // 发送文件数据
        transferFileData(socket);
    }
}

void MainWindow::transferFileData(QTcpSocket *socket)
{

    qint64 payloadSize = 1024*1; //每一帧发送1024*64个字节，控制每次读取文件的大小，从而传输速度
    double progressByte= 0;//发送进度
    qint64 bytesWritten=0;//已经发送的字节数

    while(bytesWritten != m_sendFileSize) {
        double temp = bytesWritten/1.0/m_sendFileSize*100;
        int  progress = static_cast<int>(bytesWritten/1.0/m_sendFileSize*100);
        if(bytesWritten<m_sendFileSize){

            QByteArray DataInfoBlock = m_localFile.read(qMin(m_sendFileSize,payloadSize));

            qint64 WriteBolockSize = socket->write(DataInfoBlock, DataInfoBlock.size());

            //QThread::msleep(1); //添加延时，防止服务端发送文件帧过快，否则发送过快，客户端接收不过来，导致丢包
            QThread::usleep(3); //添加延时，防止服务端发送文件帧过快，否则发送过快，客户端接收不过来，导致丢包
            //等待发送完成，才能继续下次发送，
            if(!socket->waitForBytesWritten(3*1000)) {
                ui->textBrowser->append("网络请求超时");
                return;
            }
            bytesWritten += WriteBolockSize;
            // ui->sendProgressBar->setValue(progress);
        }

        if(bytesWritten==m_sendFileSize){
            //LogWrite::LOG_DEBUG(QString("当前更新进度：100%,发送总次数:%1").arg(count), "server_"+socket->localAddress().toString());
            ui->textBrowser->append(QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(m_sendFileSize).arg(progress));
            ui->textBrowser->append(QString("-------------对[%1]的文件传输完成！------------------").arg(socket->peerAddress().toString()));
            // ui->sendProgressBar->setValue(100);
            m_localFile.close();
            return;
        }
        if(bytesWritten > m_sendFileSize) {
            ui->textBrowser->append("意外情况！！！");
            return;
        }

        if(bytesWritten/1.0/m_sendFileSize > progressByte) {
            ui->textBrowser->append(QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(m_sendFileSize).arg(progress));
            progressByte+=0.1;
        }

    }

}

void MainWindow::transferFileInfo(QTcpSocket *socket)
{
    //获取文件数据，准备发送
    QByteArray  DataInfoBlock = getFileContent(ui->pathLineEdit->text());

    QThread::msleep(10); //添加延时
    m_fileInfoWriteBytes = socket->write(DataInfoBlock) - typeMsgSize;
    qDebug()<< "传输文件信息，大小："<< m_sendFileSize;
    //等待发送完成，才能继续下次发送，否则发送过快，对方无法接收
    if(!socket->waitForBytesWritten(10*1000)) {
        ui->textBrowser->append(QString("网络请求超时,原因：%1").arg(socket->errorString()));
        return;
    }

    ui->textBrowser->append(QString("文件信息发送完成，开始对[%1]进行文件传输------------------")
                                .arg(socket->localAddress().toString()));
    qDebug()<<"当前文件传输线程id:"<<QThread::currentThreadId();

    m_localFile.setFileName(m_sendFilePath);
    if(!m_localFile.open(QFile::ReadOnly)){
        ui->textBrowser->append(QString("文件[%1]打开失败！").arg(m_sendFilePath));
        return;
    }
}

QByteArray MainWindow::getFileContent(QString filePath)
{
    if(!QFile::exists(filePath)) {
        ui->textBrowser->append(QString("没有要传输的文件！" + filePath));
        return "";
    }
    m_sendFilePath = filePath;
    ui->textBrowser->append(QString("正在获取文件信息[%1]......").arg(filePath));
    QFileInfo info(filePath);

    //获取要发送的文件大小
    m_sendFileSize = info.size();

    ui->textBrowser->append(QString("要发送的文件大小：%1字节，%2M").arg(m_sendFileSize).arg(m_sendFileSize/1024/1024.0));

    //获取发送的文件名
    QString currentFileName=filePath.right(filePath.size()-filePath.lastIndexOf('/')-1);
    QByteArray DataInfoBlock;

    QDataStream sendOut(&DataInfoBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_12);
    int type = MsgType::FileInfo;
    //封装发送的信息到DataInfoBlock中
    //消息类型             文件名                  //文件大小
    sendOut<<int(type)<<QString(currentFileName)<<qint64(m_sendFileSize);

    ui->textBrowser->append(QString("文件[%1]信息获取完成！").arg(currentFileName));
    //发送的文件总大小中，信息类型不计入
    QString msg;
    if(m_sendFileSize>1024*1024) {
        msg = QString("%1M").arg(m_sendFileSize/1024/1024.0);
    }
    else {
        msg = QString("%1KB").arg(m_sendFileSize/1024.0);
    }
    ui->textBrowser->append(QString("发送的文件名：%1，文件大小：%2").arg(currentFileName).arg(msg));

    return DataInfoBlock;
}

void MainWindow::on_actUpload_triggered(){

    if(!checkFile(ui->pathLineEdit->text())) {
        return;
    }

    QString filePath = ui->pathLineEdit->text();
    QFile file(filePath);
    QString msg;
    if(file.size()>1024*1024) {
        msg = QString("大小：%1M").arg(file.size()/1024/1024.0);
    }
    else {
        msg = QString("大小：%1KB").arg(file.size()/1024.0);
    }
    ui->textBrowser->append(msg);

}


void MainWindow::on_actListen_triggered(){


    m_tcpServer->listen(QHostAddress::Any, spinPortEdit->text().toInt());
    connect(m_tcpServer, &QTcpServer::newConnection, this, &MainWindow::delNewConnect);
    ui->actListen->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
    ui->textBrowser->append(QString("服务器已启动，监听端口：%1").arg(spinPortEdit->text()));
}


void MainWindow::on_actDisconnect_triggered(){

    m_tcpServer->close();
    ui->actListen->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
    ui->textBrowser->append(QString("服务器已关闭..."));
}




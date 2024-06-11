#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QModelIndex>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QLabel>
#include <QSpinBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QHostAddress>
#include <QMessageBox>
#include <QDataStream>//


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    myFile = new MyFileInfo(this);
    m_downloadPath = QCoreApplication::applicationDirPath() + "/../下载";
    isDownloading = false;
    QDir dir;
    if(!dir.exists(m_downloadPath)) {
        dir.mkdir(m_downloadPath);
    }

    // connectSigSlots();
    //

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

    //  工具栏
    ui->toolBar->addWidget(new QLabel("服务器地址: "));
    lineServerIp = new QLineEdit;
    lineServerIp->setText("127.0.0.1");
    ui->toolBar->addWidget(lineServerIp);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("端口号: "));
    spinPortEdit = new QSpinBox();
    spinPortEdit->setRange(0,65535);
    spinPortEdit->setValue(5555);
    ui->toolBar->addWidget(spinPortEdit);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("下载进度: "));
    downloadProgressBar = new QProgressBar();
    downloadProgressBar->setValue(0);
    ui->toolBar->addWidget(downloadProgressBar);


    connectToServer();

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


    //connectSigSlots
    connect(ui->actDownload, &QAction::triggered, this, &MainWindow::downLoadFile);
    connect(ui->actConnect, &QAction::triggered, this, [=](){
        connectToServer(m_tcpSocket);
    });
    connect(ui->actDisconnect, &QAction::triggered, this, [=](){
        m_tcpSocket->disconnectFromHost();
        ui->textBrowser->append("与服务器断开连接...");
    });
    // connect(ui->openFolder, &QPushButton::clicked, this, [=]() {
    //     QString path = QCoreApplication::applicationDirPath()+"/../下载";
    //     QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
    // });
    // connect(ui->resetProgress, &QPushButton::clicked, [=]() {
    //     ui->progressBar->setValue(0);
    // }
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

//
void MainWindow::downLoadFile()
{
    if(m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        if(!connectToServer(m_tcpSocket)) {
            return;
        }
    }
    QByteArray data;
    int typeMsg = MsgType::FileInfo;
    QDataStream out(&data, QIODevice::WriteOnly);
    //m_loadProgressBar->show();
    //发送要下载的文件列表路径
    out << typeMsg;
    ui->textBrowser->append(QString("发送消息：%1").arg(typeMsg));
    m_tcpSocket->write(data);
}

void MainWindow::readServerMsg()
{
    //如果正在下载，则收到的全是文件数据，读取即可
    if(isDownloading) {
        fileDataRead();
        return;
    }
    qDebug()<< ".............readServerMsg................";

    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_12);
    int type;
    in >> type; //判断消息类型

    if(type == MsgType::FileInfo) {
        fileInfoRead();
        isDownloading = true;
    }
    else {
        qDebug()<<"收到其他消息类型！！！type："<<type;
    }
}

void MainWindow::fileInfoRead()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_12);

    qDebug()<<"文件信息读取on_fileInfoRead......";
    // 接收文件大小，数据总大小信息和文件名大小,文件名信息
    in >> myFile->fileName >> myFile->fileSize;

    // 获取文件名，建立文件
    ui->textBrowser->append(QString("下载文件 %1, 文件大小：%2").arg(myFile->fileName).arg(myFile->fileSize));
    QString filePath = m_downloadPath + "/" + myFile->fileName;
    myFile->localFile.setFileName(filePath);
    // 打开文件，准备写入
    if(!myFile->localFile.open(QIODevice::WriteOnly)) {
        qDebug()<<"文件打开失败！";
    }
    //文件信息获取完成，接着获取文件数据
    QByteArray data;
    int typeMsg = MsgType::FileData;
    QDataStream out(&data, QIODevice::WriteOnly);
    //m_loadProgressBar->show();
    //发送要下载的文件列表路径
    out << typeMsg;
    m_tcpSocket->write(data);
}


void MainWindow::fileDataRead()
{
    qint64 readBytes = m_tcpSocket->bytesAvailable();
    if(readBytes <0) return;

    int progress = 0;
    // 如果接收的数据大小小于要接收的文件大小，那么继续写入文件
    if(myFile->bytesReceived < myFile->fileSize) {
        // 返回等待读取的传入字节数
        QByteArray data = m_tcpSocket->read(readBytes);
        myFile->bytesReceived+=readBytes;
        ui->textBrowser->append(QString("接收进度：%1/%2(字节)").arg(myFile->bytesReceived).arg(myFile->fileSize));
        progress =static_cast<int>(myFile->bytesReceived*100/myFile->fileSize);
        myFile->progressByte = myFile->bytesReceived;
        myFile->progressStr = QString("%1").arg(progress);
        downloadProgressBar->setValue(progress);
        myFile->localFile.write(data);
    }

    // 接收数据完成时
    if (myFile->bytesReceived==myFile->fileSize){
        ui->textBrowser->append(tr("接收文件[%1]成功！").arg(myFile->fileName));
        progress = 100;
        myFile->localFile.close();

        ui->textBrowser->append(QString("接收进度：%1/%2（字节）").arg(myFile->bytesReceived).arg(myFile->fileSize));
        myFile->progressByte = myFile->bytesReceived;
        downloadProgressBar->setValue(progress);
        isDownloading = false;
        myFile->initReadData();
    }

    if (myFile->bytesReceived > myFile->fileSize){
        qDebug()<<"myFile->bytesReceived > m_fileSize";
    }
}

void MainWindow::connectToServer()
{
    m_tcpSocket = new QTcpSocket(this);
    connectToServer(m_tcpSocket);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::readServerMsg);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, [=]() {
        ui->textBrowser->append(QString("与服务器断开连接：原因：%1").arg(m_tcpSocket->errorString()));
        //isConnected = false;
    });
}

bool MainWindow::connectToServer(QTcpSocket *socket)
{
    socket->connectToHost(lineServerIp->text(),spinPortEdit->text().toInt());
    if(!socket->waitForConnected(2*1000)) {
        QMessageBox::warning(this, "警告", "服务器连接失败，原因："+m_tcpSocket->errorString());
        return false;
    }
    QMessageBox::information(this, "提示", "服务器连接成功！");
    ui->textBrowser->append("服务器连接成功！");

    return true;
}

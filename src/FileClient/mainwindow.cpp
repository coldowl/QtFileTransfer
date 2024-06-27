#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpclient.h"
#include "fileclient.h"
#include "custommessagehandler.h"

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
#include <QDataStream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 将全局指针指向textBrowser
    G_TextBrowser = ui->textBrowser;

    // 安装自定义消息处理器
    qInstallMessageHandler(customMessageHandler);

    model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);
    ui->treeView_2->setHeaderHidden(true);

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

    // ui->toolBar->addSeparator();
    // ui->toolBar->addWidget(new QLabel("下载进度: "));
    // downloadProgressBar = new QProgressBar();
    // downloadProgressBar->setValue(0);
    // ui->toolBar->addWidget(downloadProgressBar);


    // treeView中选择文件夹，listView展示文件夹中的内容
    connect(ui->treeView, &QTreeView::clicked, this, [=](const QModelIndex &index) {
        ui->listView->setRootIndex(index);
        ui->datailTextEdit->clear();//清除文本窗口
    });

    // 连接listView的clicked信号到显示详细信息的槽函数
    connect(ui->listView, &QListView::clicked, this, &MainWindow::onListViewClicked);

    // 连接服务器listview的clicked信号到显示详细信息的槽函数
    connect(ui->treeView, &QListView::clicked, this, &MainWindow::onListViewClicked);


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


void MainWindow::on_actConnect_triggered(){

    bool isConnect;

    m_tcpClient = new TcpClient();
    m_fileClient = new FileClient(m_tcpClient, ui->treeView_2);

    isConnect = m_tcpClient->connectToServer(lineServerIp->text(), spinPortEdit->value()); // 连接到服务器

    if(isConnect){
        ui->actConnect->setEnabled(false);
        ui->actDisconnect->setEnabled(true);
        ui->actUpload->setEnabled(true);
        ui->actDownload->setEnabled(true);
        ui->actDelete->setEnabled(true);
        ui->textBrowser->append(QString("已连接 %1:%2 ").arg(lineServerIp->text()).arg(spinPortEdit->value()));
    }else{
        ui->textBrowser->append("连接失败！");
    }
}



void MainWindow::on_actDisconnect_triggered(){
    m_tcpClient->disconnectFromServer();
    ui->textBrowser->append("断开连接！");
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}


void MainWindow::on_actUpload_triggered()
{
    m_fileClient->requestUpload(ui->pathLineEdit->text());
}


void MainWindow::on_pushButton_clicked()
{
    m_fileClient->requestFileTree();
}


void MainWindow::on_actDelete_triggered(){
    QModelIndex index = ui->treeView_2->currentIndex();
    qDebug() << "Triggered download, current index:" << index;
    if (index.isValid()) {
        QAbstractItemModel *model = ui->treeView_2->model();
        QString fileName = model->data(index, Qt::DisplayRole).toString();
        qDebug() << "Selected file:" << fileName;
        // 调用请求删除函数并传递文件名作为参数
        m_fileClient->requestDelete(fileName);
    } else {
        qDebug() << "No file selected";
    }
}


void MainWindow::on_actDownload_triggered() {
    QModelIndex index = ui->treeView_2->currentIndex();
    qDebug() << "Triggered download, current index:" << index;
    if (index.isValid()) {
        QAbstractItemModel *model = ui->treeView_2->model();
        QString fileName = model->data(index, Qt::DisplayRole).toString();
        qDebug() << "Selected file:" << fileName;
        // 调用请求下载函数并传递文件名作为参数
        m_fileClient->requestDownload(fileName);
    } else {
        qDebug() << "No file selected";
    }
}



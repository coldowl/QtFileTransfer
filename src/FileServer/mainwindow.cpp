#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpserver.h"
// #include "udpserver.h"
#include "fileserver.h"
#include "custommessagehandler.h"
#include "mediator.h"

#include <QDir>
#include <QModelIndex>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QLabel>
#include <QTimer>
#include <QSpinBox>
#include <QString>
#include <QHostAddress>
#include <QMessageBox>
#include <QFileInfo>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tcpServer(new TcpServer(this))
    , m_udpServer(new UdpServer(this))
    , m_fileServer(new FileServer(this))
    , m_ppf(new ProtocolPacketFactory(this))
    , m_dpf(new DataPacketFactory(this))
{
    ui->setupUi(this);


    // 多线程
    QThread* sub = new QThread; // 创建线程对象
    m_fileServer = new FileServer(); // 创建工作的类对象
    m_fileServer->moveToThread(sub); // 将工作的类对象移动到创建的子线程对象中
    sub->start();// 启动线程,但是线程工作要等有人调用它的槽函数

    // // 将全局指针指向textBrowser
    // G_TextBrowser = ui->textBrowser;

    // // 安装自定义消息处理器
    // qInstallMessageHandler(customMessageHandler);

    m_model = new QFileSystemModel(this);
    m_model -> setRootPath("C:/Users/COLDOWL/Desktop/open"); // 方便调试
    // model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(m_model);
    ui->treeView->setHeaderHidden(true);

    QList<int> columnsToHide = {1, 2, 3};  // 定义一个需要隐藏的列的列表
    for (int column : columnsToHide) {  // 使用循环来隐藏列
        ui->treeView->setColumnHidden(column, true); //依次隐藏 Size、Type、Date Modified 列
    }

    ui->listView->setModel(m_model);

    //状态栏
    m_labSocketState=new QLabel("Socket状态：未获取");//状态栏标签
    m_labSocketState->setMinimumWidth(250);
    ui->statusbar->addWidget(m_labSocketState);

    QString localIP=getLocalIP();//本机IP
    QLabel *LabIpState=new QLabel("IP地址："+localIP);//状态栏标签
    LabIpState->setMinimumWidth(250);
    ui->statusbar->addWidget(LabIpState);

    //工具栏
    ui->toolBar->addSeparator();//分隔条
    ui->toolBar->addWidget(new QLabel("端口号: "));
    m_spinPortEdit = new QSpinBox();
    m_spinPortEdit->setRange(0,65535);
    m_spinPortEdit->setValue(5555);

    ui->toolBar->addWidget(m_spinPortEdit);

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
        QString path = m_model->filePath(current);
        ui->pathLineEdit->setText(path);
    };
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);

    // 中介者管理connect函数
    Mediator mediator(m_fileServer, m_tcpServer, m_udpServer, m_ppf, m_dpf);
    connect(m_tcpServer, &TcpServer::stateChanged, this, &MainWindow::onSocketStateChanged);



}

MainWindow::~MainWindow()
{
    delete ui;
}


//获取本地IP
QString MainWindow::getLocalIP()
{
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString localIP="";

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
    QString path = m_model->filePath(index);    // 获取点击的项对应的路径

    if (m_model->isDir(index)) { // 检查点击的项是否为目录

        ui->listView->setRootIndex(index);// 进入文件夹，更新listView

        QDir dir(path);// 获取该路径对应的父文件夹路径
        dir.cdUp();
        QString parentPath = dir.absolutePath();


        QModelIndex parentIndex = m_model->index(parentPath);// 根据父文件夹路径找到对应的索引

        ui->treeView->expand(parentIndex);// 在treeView展开父文件夹
    }



    // 2.打印文件详细信息到文本窗口
    QFileInfo fileInfo = m_model->fileInfo(index);
    QString details;

    if (fileInfo.isDir()) {
        details = QString("Directory:\n %1").arg(fileInfo.filePath());
    } else {
        details = QString("File:\n %1\n" "Size:\n %2\n" "Type:\n%3\n" "Last Modified:\n %4")
                      .arg(fileInfo.filePath())
                      .arg(fileInfo.size())
                      .arg(fileInfo.suffix().isEmpty() ? "Unknown" : fileInfo.suffix())
                      .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    }

    ui->datailTextEdit->setText(details);

}


void MainWindow::on_actListen_triggered(){

    m_tcpServer->listen(QHostAddress::Any, m_spinPortEdit->value());
    ui->actListen->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
    ui->textBrowser->append(QString("正在监听 %1 端口...").arg(m_spinPortEdit->value()));
}


void MainWindow::on_actDisconnect_triggered(){
    m_tcpServer->close();
    ui->textBrowser->append("关闭服务器");
    ui->actListen->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}


void MainWindow::on_actChooseFolder_triggered(){
    QFileInfo fileinfo(ui->pathLineEdit->text());
    if(fileinfo.isDir()){
        m_fileServer->setOpenFolder(ui->pathLineEdit->text());
    }
    else{
        ui->textBrowser->append("请选择一个文件夹");
    }
}


// 展示套接字状态变化
void MainWindow::onSocketStateChanged(QString stateString) {
    m_stateQueue.append(stateString);

    if (!m_processing) {
        m_processing = true;
        processNextState();
    }
}

// 使用队列缓存套接字状态
void MainWindow::processNextState() {
    if (!m_stateQueue.isEmpty()) {
        QString nextState = m_stateQueue.takeFirst();
        m_labSocketState->setText(nextState);

        QTimer::singleShot(500, this, &MainWindow::processNextState);
    } else {
        m_processing = false;
    }
}

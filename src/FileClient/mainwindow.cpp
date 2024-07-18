#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpclient.h"
#include "fileclient.h"
#include "custommessagehandler.h"
#include "mediator.h"

#include <QDir>
#include <QModelIndex>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QHostAddress>
#include <QMessageBox>
#include <QDataStream>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // , m_tcpClient(new TcpClient(this))
    // , m_udpClient(new UdpClient(this))
    , m_fileClient(new FileClient(this))
    , m_ppf(new ProtocolPacketFactory(this))
    , m_dpf(new DataPacketFactory(this))
    // , m_fileTransferWidget(new FileTransferWidget(this))
{
    ui->setupUi(this);

    // 加载QSS文件
    QFile file(":/qss/mainwindow.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Could not open style.qss";
    }


    // 多线程
    // 文件操作线程
    QThread* m_fileThread = new QThread; // 创建线程对象
    m_fileClient = new FileClient(); // 创建工作的类对象
    m_fileClient->moveToThread(m_fileThread); // 将工作的类对象移动到创建的子线程对象中


    // TCP通信线程
    QThread* m_tcpThread = new QThread;
    m_tcpClient = new TcpClient();
    m_tcpClient->moveToThread(m_tcpThread);

    // UDP通信线程
    QThread* m_udpThread = new QThread;
    m_udpClient = new UdpClient();
    m_udpClient->moveToThread(m_udpThread);



    connect(m_tcpThread, &QThread::started, m_tcpClient, &TcpClient::init);
    connect(m_udpThread, &QThread::started, m_udpClient, &UdpClient::init);
    connect(m_fileThread, &QThread::started, m_fileClient, &FileClient::init);

    connect(m_fileThread, &QThread::finished, m_fileClient, &QObject::deleteLater);
    connect(m_tcpThread, &QThread::finished, m_tcpClient, &QObject::deleteLater);
    connect(m_udpThread, &QThread::finished, m_udpClient, &QObject::deleteLater);

    // 启动线程
    m_tcpThread->start();
    m_udpThread->start();
    m_fileThread->start();


    // 使用信号槽确保在 init 之后再调用成员方法
    connect(this, &MainWindow::startTcpConnect, m_tcpClient, &TcpClient::connectToServer);
    connect(this, &MainWindow::startUdpConnect, m_udpClient, &UdpClient::setConnectInfo);
    connect(this, &MainWindow::tcpDisconnect, m_tcpClient, &TcpClient::disconnectFromServer);
    connect(this, &MainWindow::udpDisconnect, m_udpClient, &UdpClient::closeUdpSocket);
    connect(this, &MainWindow::requestUpload, m_fileClient, &FileClient::requestUpload);
    connect(this, &MainWindow::requestFileTree, m_fileClient, &FileClient::requestFileTree);
    connect(this, &MainWindow::requestDelete, m_fileClient, &FileClient::requestDelete);
    connect(this, &MainWindow::requestDownload, m_fileClient, &FileClient::requestDownload);


    m_fileTransferWidget = new FileTransferWidget();


    // // 将全局指针指向textBrowser
    // G_TextBrowser = ui->textBrowser;

    // // 安装自定义消息处理器
    // qInstallMessageHandler(customMessageHandler);

    model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);
    ui->treeView_2->setHeaderHidden(true);

    QList<int> columnsToHide = {1, 2, 3};  // 定义一个需要隐藏的列的列表
    for (int column : columnsToHide) {  // 使用循环来隐藏列
        ui->treeView->setColumnHidden(column, true); // 依次隐藏 Size、Type、Date Modified 列
    }

    ui->listView->setModel(model);

    //状态栏
    LabSocketState=new QLabel("Socket状态：未获取");//状态栏标签
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
    spinPortEdit = new QSpinBox;
    spinPortEdit->setRange(0,65535);
    spinPortEdit->setValue(5555);
    ui->toolBar->addWidget(spinPortEdit);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("选择传输协议: "));
    protocolComboBox = new QComboBox(this);
    protocolComboBox->addItem("TCP");
    protocolComboBox->addItem("UDP");
    protocolComboBox->setCurrentIndex(0); // 默认选择TCP
    ui->toolBar->addWidget(protocolComboBox);


    // 连接协议选择的信号槽
    connect(protocolComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onProtocolChanged);

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

    // 中介者管理connect函数
    Mediator mediator(m_fileClient, m_tcpClient, m_udpClient, m_fileTransferWidget, m_ppf, m_dpf, m_selectedProtocol);

    connect(m_tcpClient, &TcpClient::stateChanged, this, &MainWindow::onSocketStateChanged);

    // 展示文件树
    connect(m_fileClient, &FileClient::fileTreeModel, this, [this](QStandardItemModel *model){
        ui->treeView_2->setModel(model);
    });


    // 使用 lambda 表达式处理 TcpClient 和 UdpClient 的 success 信号
    auto connectSuccessHandler = [this]() {
        protocolComboBox->setEnabled(false);
        ui->actConnect->setEnabled(false);
        ui->actDisconnect->setEnabled(true);
        ui->actUpload->setEnabled(true);
        ui->actDownload->setEnabled(true);
        ui->actDelete->setEnabled(true);
        ui->textBrowser->append(QString("已连接 %1:%2 ").arg(lineServerIp->text()).arg(spinPortEdit->value()));
    };

    // 只要有一个连接成功, 调用successHandler
    connect(m_tcpClient, &TcpClient::tcpConnectSuccess, this, connectSuccessHandler);
    connect(m_udpClient, &UdpClient::setUdpConnectInfoSuccess, this, connectSuccessHandler);

    // 连接失败
    connect(m_tcpClient, &TcpClient::tcpConnectFailed, this, [this](){
        ui->textBrowser->append("TCP连接失败！");
    });
    connect(m_udpClient, &UdpClient::setUdpConnectInfoFailed, this, [this](){
        ui->textBrowser->append("UDP设置连接失败，检查ip和端口号格式是否正确");
    });




    auto disconnectSuccessHander = [this](){
        protocolComboBox->setEnabled(true);
        ui->actConnect->setEnabled(true);
        ui->actDisconnect->setEnabled(false);
    };

    connect(m_tcpClient, &TcpClient::tcpDisconnectSuccess, this, disconnectSuccessHander);
    connect(m_udpClient, &UdpClient::udpCloseSuccess, this, disconnectSuccessHander);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_fileTransferWidget;
    m_tcpThread->quit();
    m_tcpThread->wait();
    m_udpThread->quit();
    m_tcpThread->wait();
    m_fileThread->quit();
    m_fileThread->wait();
}

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
    QLocale locale = this->locale();

    if (fileInfo.isDir()) {
        details = QString("Directory:\n %1").arg(fileInfo.filePath());
    } else {
        details = QString("File:\n  %1\n" "Size:\n  %2\n" "Type:\n  %3\n" "Last Modified:\n  %4\n")
                      .arg(fileInfo.filePath())
                      .arg(locale.formattedDataSize(fileInfo.size()))
                      .arg(fileInfo.suffix().isEmpty() ? "Unknown" : fileInfo.suffix())
                      .arg(fileInfo.lastModified().toString(Qt::DefaultLocaleLongDate));

    }

    ui->datailTextEdit->setText(details);

}

void MainWindow::on_actConnect_triggered(){

    if (m_selectedProtocol == 0) { // 选择TCP
        emit startTcpConnect(lineServerIp->text(), spinPortEdit->value());
    } else if (m_selectedProtocol == 1) { // 选择UDP
        emit startUdpConnect(lineServerIp->text(), spinPortEdit->value());
    }

}

void MainWindow::on_actDisconnect_triggered(){

    if (m_selectedProtocol == 0) { // TCP

        emit tcpDisconnect(); // 断开TCP连接
        ui->textBrowser->append("已断开TCP连接！");

    } else if (m_selectedProtocol == 1) { // UDP

        emit udpDisconnect(); // 关闭UDP套接字
        ui->textBrowser->append("已关闭UDP套接字！");
    }

    // auto successHander = [this](){
    //     protocolComboBox->setEnabled(true);
    //     ui->actConnect->setEnabled(true);
    //     ui->actDisconnect->setEnabled(false);
    // };

    // connect(m_tcpClient, &TcpClient::tcpDisconnectSuccess, this, successHander);
    // connect(m_udpClient, &UdpClient::udpCloseSuccess, this, successHander);

}

void MainWindow::on_actUpload_triggered(){

    // m_fileTransferWidget = new FileTransferWidget();
    // connect(m_fileClient, &FileClient::uploadBasicInfo, m_fileTransferWidget, &FileTransferWidget::setBasicInfo);
    // connect(m_fileClient, &FileClient::uploadProgressInfo, m_fileTransferWidget, &FileTransferWidget::setProgressInfo);
    // 弹出进度窗口
    emit requestUpload(ui->pathLineEdit->text());
    // m_fileClient->requestUpload(ui->pathLineEdit->text());

    // fileTransferWidget->setWindowModality(Qt::ApplicationModal); // 设置为模态对话框
    // m_fileTransferWidget->setAttribute(Qt::WA_DeleteOnClose); // 关闭的时候删除

    m_fileTransferWidget->show();


}

void MainWindow::on_refreshButton_clicked(){
    emit requestFileTree();
    // m_fileClient->requestFileTree();
}

void MainWindow::on_actDelete_triggered(){
    QModelIndex index = ui->treeView_2->currentIndex();
    qDebug() << "Triggered download, current index:" << index;
    if (index.isValid()) {
        QAbstractItemModel *model = ui->treeView_2->model();
        QString fileName = model->data(index, Qt::DisplayRole).toString();
        qDebug() << "Selected file:" << fileName;
        // 调用请求删除函数并传递文件名作为参数
        emit requestDelete(fileName);
        // m_fileClient->requestDelete(fileName);
    } else {
        qDebug() << "未选择任何文件！";
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
        emit requestDownload(fileName);
        // m_fileClient->requestDownload(fileName);
    } else {
        qDebug() << "未选中任何文件！";
    }
}

void MainWindow::onProtocolChanged(int index)
{
    QString selectedProtocol = protocolComboBox->itemText(index);
    if (selectedProtocol == "TCP") {
        // 切换到 TCP 处理
        m_selectedProtocol = 0;
        connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_tcpClient, &TcpClient::enqueuePacket, Qt::UniqueConnection);
        disconnect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_udpClient, &UdpClient::sendDatagram);
        ui->textBrowser->append("切换到 TCP 处理");

    } else if (selectedProtocol == "UDP") {
        // 切换到 UDP 处理
        m_selectedProtocol = 1;
        connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_udpClient, &UdpClient::sendDatagram, Qt::UniqueConnection);
        disconnect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_tcpClient, &TcpClient::enqueuePacket);

        ui->textBrowser->append("切换到 UDP 处理");
    }
}


// 展示其它类传递的消息
void MainWindow::displayState(const QString &message){


}

// 展示套接字状态变化
void MainWindow::onSocketStateChanged(QString stateString) {
    stateQueue.append(stateString);

    if (!processing) {
        processing = true;
        processNextState();
    }
}

// 使用队列缓存套接字状态
void MainWindow::processNextState() {
    if (!stateQueue.isEmpty()) {
        QString nextState = stateQueue.takeFirst();
        LabSocketState->setText(nextState);

        QTimer::singleShot(500, this, &MainWindow::processNextState);
    } else {
        processing = false;
    }
}




#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpclient.h"
#include "fileclient.h"
#include "treeviewdelegate.h"
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

    m_fileTransferWidget = new FileTransferWidget();

    initUi();
    threadHandle();
    setConnect();
}

MainWindow::~MainWindow(){
    delete ui;
    delete m_fileTransferWidget;
}

QString MainWindow::getLocalIP(){
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();

    if (!addList.isEmpty())
        for (int i = 0; i < addList.count(); i++){
            QHostAddress aHost=addList.at(i);
            if (QAbstractSocket::IPv4Protocol==aHost.protocol()){
                localIP=aHost.toString();
                break;
            }
        }
    return localIP;
}

void MainWindow::onListViewClicked(const QModelIndex &index){
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
        emit startTcpConnect(m_lineServerIp->text(), m_spinPortEdit->value());
    } else if (m_selectedProtocol == 1) { // 选择UDP
        emit startUdpConnect(m_lineServerIp->text(), m_spinPortEdit->value());
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

}

void MainWindow::on_actUpload_triggered(){

    emit requestUpload(ui->pathLineEdit->text());
    // m_fileClient->requestUpload(ui->pathLineEdit->text());

    // fileTransferWidget->setWindowModality(Qt::ApplicationModal); // 设置为模态对话框
    // m_fileTransferWidget->setAttribute(Qt::WA_DeleteOnClose); // 关闭的时候删除
    // m_fileTransferWidget = new FileTransferWidget();
    bool isUpload = true;
    m_fileTransferWidget->setPartiesName(isUpload);
    m_fileTransferWidget->show();

}

void MainWindow::on_refreshButton_clicked(){
    emit requestFileTree();
    // m_fileClient->requestFileTree();
}

void MainWindow::on_actDelete_triggered(){
    QModelIndex index = ui->treeView_2->currentIndex();
    qDebug() << "触发删除，当前索引:" << index;
    if (index.isValid()) {
        QModelIndex siblingIndex = index.sibling(index.row(), 0); // 获取同一行的第一列项
        QString fileName = siblingIndex.data().toString();
        // qDebug() << "Selected file:" << fileName;

        emit requestDelete(fileName); // 调用请求删除函数并传递文件名作为参数
        // m_fileClient->requestDelete(fileName);

    } else {
        qDebug() << "未选择任何文件！";
    }
}

void MainWindow::on_actDownload_triggered() {
    QModelIndex index = ui->treeView_2->currentIndex();
    qDebug() << "触发下载，当前索引:" << index;
    if (index.isValid()) {
        QModelIndex siblingIndex = index.sibling(index.row(), 0); // 获取同一行的第一列项
        QString fileName = siblingIndex.data().toString();
        // qDebug() << "Selected file:" << fileName;


        emit requestDownload(fileName); // 调用请求下载函数并传递文件名作为参数
        bool isUpload = false;
        m_fileTransferWidget->setPartiesName(isUpload);
        m_fileTransferWidget->show();

    } else {
        qDebug() << "未选中任何文件！";
    }
}

void MainWindow::onProtocolChanged(int index)
{
    QString selectedProtocol = m_protocolComboBox->itemText(index);
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

// 处理Treeview2的删除、下载操作
void MainWindow::onTreeViewItemClicked(const QModelIndex &index)
{
    if (index.column() == 1) { // 下载
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认下载", "你确定要下载这个文件吗？",QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            emit readyForDownload();
        }
    }
    else if (index.column() == 2) { // 删除
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认删除", "你确定要删除这个文件吗？",QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            emit readyForDelete();
        }
    }
}



void MainWindow::initUi(){
    // 加载QSS文件
    qApp->setStyleSheet("file:///:/qss/mainwindow.qss");

    ui->pathHorLayout->setSpacing(0); // 设置间距为 0

    this->setWindowIcon(QIcon(":/icons/logo.png"));
    this->setWindowTitle("文件传输工具");

    m_model = new QFileSystemModel(this);
    m_model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(m_model);
    ui->treeView->setHeaderHidden(true);
    ui->treeView_2->setHeaderHidden(true);
    // ui->treeView_2->setItemDelegate(new TreeViewDelegate(ui->treeView_2));

    QList<int> columnsToHide = {1, 2, 3};  // 定义一个需要隐藏的列的列表
    for (int column : columnsToHide) {  // 使用循环来隐藏列
        ui->treeView->setColumnHidden(column, true); // 依次隐藏 Size、Type、Date Modified 列
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

    //  工具栏
    ui->toolBar->addWidget(new QLabel("服务器地址: "));
    m_lineServerIp = new QLineEdit;
    m_lineServerIp->setText("127.0.0.1");
    ui->toolBar->addWidget(m_lineServerIp);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("端口号: "));
    m_spinPortEdit = new QSpinBox;
    m_spinPortEdit->setRange(0,65535);
    m_spinPortEdit->setValue(5555);
    ui->toolBar->addWidget(m_spinPortEdit);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("选择传输协议: "));
    m_protocolComboBox = new QComboBox(this);
    m_protocolComboBox->addItem("TCP");
    m_protocolComboBox->addItem("UDP");
    m_protocolComboBox->setCurrentIndex(0); // 默认选择TCP
    ui->toolBar->addWidget(m_protocolComboBox);

}

void MainWindow::threadHandle(){
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

    // 文件传输线程
    QThread* m_syncThread = new QThread;
    m_syncFileFileTransferUI = new SyncFileTransferUI();
    m_syncFileFileTransferUI->moveToThread(m_syncThread);

    connect(m_tcpThread, &QThread::started, m_tcpClient, &TcpClient::init);
    connect(m_udpThread, &QThread::started, m_udpClient, &UdpClient::init);
    connect(m_fileThread, &QThread::started, m_fileClient, &FileClient::init);

    connect(m_fileThread, &QThread::finished, m_fileClient, &QObject::deleteLater);
    connect(m_tcpThread, &QThread::finished, m_tcpClient, &QObject::deleteLater);
    connect(m_udpThread, &QThread::finished, m_udpClient, &QObject::deleteLater);
    connect(m_syncThread, &QThread::finished, m_syncFileFileTransferUI, &QObject::deleteLater);

    // 启动线程
    m_tcpThread->start();
    m_udpThread->start();
    m_fileThread->start();
    m_syncThread->start();
}

void MainWindow::setConnect(){

    connect(ui->treeView_2, &QTreeView::clicked, this, &MainWindow::onTreeViewItemClicked);

    // 使用信号槽确保在 init 之后再调用成员方法
    connect(this, &MainWindow::startTcpConnect, m_tcpClient, &TcpClient::connectToServer);
    connect(this, &MainWindow::startUdpConnect, m_udpClient, &UdpClient::setConnectInfo);
    connect(this, &MainWindow::tcpDisconnect, m_tcpClient, &TcpClient::disconnectFromServer);
    connect(this, &MainWindow::udpDisconnect, m_udpClient, &UdpClient::closeUdpSocket);
    connect(this, &MainWindow::requestUpload, m_fileClient, &FileClient::requestUpload);
    connect(this, &MainWindow::requestFileTree, m_fileClient, &FileClient::requestFileTree);
    connect(this, &MainWindow::requestDelete, m_fileClient, &FileClient::requestDelete);
    connect(this, &MainWindow::requestDownload, m_fileClient, &FileClient::requestDownload);

    connect(this, &MainWindow::readyForDownload, this, &MainWindow::on_actDownload_triggered);
    connect(this, &MainWindow::readyForDelete, this, &MainWindow::on_actDelete_triggered);

    // 协议选择
    connect(m_protocolComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onProtocolChanged);

    // treeView 和 listView
    connect(ui->treeView, &QTreeView::clicked, this, [=](const QModelIndex &index) {
        ui->listView->setRootIndex(index);
        ui->datailTextEdit->clear();
    });

    connect(ui->listView, &QListView::clicked, this, &MainWindow::onListViewClicked);
    connect(ui->treeView, &QListView::clicked, this, &MainWindow::onListViewClicked);

    // 当选中listView或者treeview中的项目，pathLineEdit 实时显示当前文件路径
    auto updatePath = [=](const QModelIndex &current) {
        QString path = m_model->filePath(current);        // 获取当前选定项的路径
        ui->pathLineEdit->setText(path);
    };
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, updatePath);

    // 中介者管理connect函数
    Mediator mediator(m_fileClient, m_tcpClient, m_udpClient, m_ppf, m_dpf, m_selectedProtocol);

    connect(m_tcpClient, &TcpClient::stateChanged, this, &MainWindow::onSocketStateChanged);

    // 展示文件树
    connect(m_fileClient, &FileClient::fileTreeModel, this, [this](QStandardItemModel *model){
        ui->treeView_2->setModel(model);
        QStandardItem *rootItem = model->invisibleRootItem()->child(0); // 获取根项（顶级文件夹）
        ui->lineEdit->setText(rootItem->text());
        ui->treeView_2->expandAll(); // 展开所有项以确保视图更新
        ui->treeView_2->header()->setSectionResizeMode(QHeaderView::ResizeToContents); // 确保 treeView 显示多列
    });


    // 协议连接成功
    auto connectSuccessHandler = [this]() {
        m_protocolComboBox->setEnabled(false);
        ui->actConnect->setEnabled(false);
        ui->actDisconnect->setEnabled(true);
        ui->actUpload->setEnabled(true);
        ui->actDownload->setEnabled(true);
        ui->actDelete->setEnabled(true);
        ui->textBrowser->append(QString("已连接 %1:%2 ").arg(m_lineServerIp->text()).arg(m_spinPortEdit->value()));
    };
    connect(m_tcpClient, &TcpClient::tcpConnectSuccess, this, connectSuccessHandler);
    connect(m_udpClient, &UdpClient::setUdpConnectInfoSuccess, this, connectSuccessHandler);

    // 协议连接失败
    connect(m_tcpClient, &TcpClient::tcpConnectFailed, this, [this](){
        ui->textBrowser->append("TCP连接失败！");
    });
    connect(m_udpClient, &UdpClient::setUdpConnectInfoFailed, this, [this](){
        ui->textBrowser->append("UDP设置连接失败，检查ip和端口号格式是否正确");
    });


    // 协议断连成功
    auto disconnectSuccessHander = [this](){
        m_protocolComboBox->setEnabled(true);
        ui->actConnect->setEnabled(true);
        ui->actDisconnect->setEnabled(false);
    };
    connect(m_tcpClient, &TcpClient::tcpDisconnectSuccess, this, disconnectSuccessHander);
    connect(m_udpClient, &UdpClient::udpCloseSuccess, this, disconnectSuccessHander);


    // 文件传输窗口
    connect(m_fileClient, &FileClient::uploadedFileInfo, m_syncFileFileTransferUI, &SyncFileTransferUI::updateFileInfo);
    connect(m_fileClient, &FileClient::bytesAlreadySent, m_syncFileFileTransferUI, &SyncFileTransferUI::updateProgress);
    connect(m_fileClient, &FileClient::downloadedFileInfo,m_syncFileFileTransferUI, &SyncFileTransferUI::updateFileInfo);
    connect(m_fileClient, &FileClient::bytesAlreadyRcv, m_syncFileFileTransferUI, &SyncFileTransferUI::updateProgress);
    connect(m_fileClient, &FileClient::timeStarted, m_syncFileFileTransferUI, &SyncFileTransferUI::onTimeStarted);

    connect(m_syncFileFileTransferUI, &SyncFileTransferUI::fileInfoUpdated, m_fileTransferWidget, &FileTransferWidget::setFileInfo);
    connect(m_syncFileFileTransferUI, &SyncFileTransferUI::progressUpdated, m_fileTransferWidget, &FileTransferWidget::setProgress);
    connect(m_syncFileFileTransferUI, &SyncFileTransferUI::speedUpdated, m_fileTransferWidget, &FileTransferWidget::setSpeed);
    connect(m_syncFileFileTransferUI, &SyncFileTransferUI::restFileSizeUpdated, m_fileTransferWidget, &FileTransferWidget::setRestFileSize);
    connect(m_syncFileFileTransferUI, &SyncFileTransferUI::finishTimeUpdated, m_fileTransferWidget, &FileTransferWidget::setFinishTime);

}




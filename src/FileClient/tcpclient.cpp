#include "tcpclient.h"
#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QThread>


// 构造函数，初始化treeView和model
TcpClient::TcpClient(QTreeView *view, QObject *parent)
    : QObject(parent), m_treeView(view) {
    model = new QStandardItemModel();
    m_treeView->setModel(model);

    m_socket = new QTcpSocket(this); // 初始化 TCP socket
    m_uploadFile = new QFile(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead); // 设置读取数据信号和槽
}

// 连接到服务器
bool TcpClient::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port); // 连接到指定的host和端口
    if (m_socket->waitForConnected()) { // 等待连接
        // requestFileList(); // 连接成功后请求文件列表
        requestFileTree(); // 连接成功后请求文件树
        return true;
    }else{
        return false;
    }
}

// 断开和服务器的连接
bool TcpClient::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            if (!m_socket->waitForDisconnected()) {
                qDebug() << "Error disconnecting:" << m_socket->errorString();
                return false;
            }
        }
    }
    return m_socket->state() == QAbstractSocket::UnconnectedState; // 返回 1 表示断开连接成功
}

// 读取服务器发送的数据
void TcpClient::onReadyRead() {
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

    ushort command;
    in >> command; // 读取指令字
    qDebug() << "收到指令字:" << command;

    switch(command) {
    case FILE_TREE: { // 文件树
        QString rootPath;
        in >> rootPath; // 读取根路径

        model->clear(); // 清除现有的model数据
        QStandardItem *rootItem = new QStandardItem(rootPath);
        model->appendRow(rootItem); // 将根路径添加到model中

        parseDirectory(in, rootItem); // 解析目录结构
        break;
    }
    case FILE_LIST: { // 文件列表
        int fileCount;
        in >> fileCount; // 读取文件数量

        model->clear(); // 清除现有的model数据
        for (int i = 0; i < fileCount; ++i) {
            QString fileName;
            in >> fileName; // 读取每个文件名
            QStandardItem *item = new QStandardItem(fileName);
            model->appendRow(item); // 将文件名添加到model中
        }
        break;
    }
    case UPLOAD_FILE_READY: { // 准备上传文件
        uploadFile();
        break;
    }
    case UPLOAD_COMPLETE: { // 上传完成
        // 上传完成后的处理
        break;
    }
    case DOWNLOAD_FILE_READY: { // 准备下载文件
        in >> m_downloadFileName >> m_downloadFileSize >> m_downloadFileHash;
        qDebug() << m_downloadFileName << m_downloadFileSize << m_downloadFileHash;
        QDir downloadDir("C:/download");
        // 创建文件路径
        if (!downloadDir.exists()) {
            downloadDir.mkpath(".");
        }
        QString filePath = downloadDir.filePath(m_downloadFileName);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            m_downloadFilePath = filePath;
            QByteArray request;
            QDataStream out(&request, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

            qDebug() << "send: RECEIVE_FILE_READY";
            out << static_cast<ushort>(RECEIVE_FILE_READY); // 指令字 RECEIVE_FILE_READY
            m_socket->write(request); // 发送请求
            m_socket->flush(); // 刷新socket
        }
        break;
    }
    case DOWNLOAD_FILE: { // 下载文件
        receiveDownload(in);
        break;
    }
    // 其他指令类型处理
    default:{
        qWarning() << "未知指令字:" << command;
        break;
    }
}
}

// 请求文件列表
void TcpClient::requestFileList() {
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

    out << static_cast<ushort>(GET_FILE_LIST); // 指令字 FILE_LIST
    m_socket->write(request); // 发送请求
    m_socket->flush(); // 刷新socket
}

// 请求文件树
void TcpClient::requestFileTree(){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);
    out << static_cast<ushort>(GET_FILE_TREE);
    m_socket->write(request); // 发送请求
    m_socket->flush(); // 刷新socket
}

// 请求上传文件
void TcpClient::requestUpload(const QString &filePath){
    m_uploadFilePath = filePath;
    QFile file(filePath);
    QFileInfo fileInfo(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file");
        return;
    }

    QByteArray fileData = file.readAll();
    QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << static_cast<ushort>(REQUEST_UPLOAD_FILE) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    out << static_cast<ushort>(REQUEST_UPLOAD_FILE) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    m_socket->write(request); // 发送请求
    m_socket->flush(); // 刷新socket


}

// 请求下载文件
void TcpClient::requestDownload(const QString &fileName){
    m_downloadFileName = fileName; // 设置成员变量m_uploadFilePath

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    out << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    m_socket->write(request); // 发送请求
    m_socket->flush(); // 刷新socket
}

// 请求删除文件
void TcpClient::requestDelete(const QString &fileName){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << static_cast<ushort>(REQUEST_DELETE_FILE) << fileName;
    out << static_cast<ushort>(REQUEST_DELETE_FILE) << fileName;
    m_socket->write(request); // 发送请求
    m_socket->flush(); // 刷新socket
}

// 上传文件
void TcpClient::uploadFile(){
    QFile file(m_uploadFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file");
        return;
    }

    QByteArray fileData = file.readAll();

    qint64 bytesSent = 0;
    int count = 0;

    while (bytesSent < fileData.size()) {
        QByteArray packet;
        QDataStream out(&packet,QIODevice::WriteOnly);
        QByteArray chunk = fileData.mid(bytesSent, 50000);

        out << static_cast<ushort>(UPLOAD_FILE) << chunk;
        qDebug() << count++ << "UPLOAD_FILE";
        bytesSent += chunk.size();

        m_socket->write(packet); // 将文件数据写入
        m_socket->waitForBytesWritten(3000); // 等待写完
        m_socket->flush(); // 刷新socket
        QThread::msleep(3); // 防止发送过快，服务器来不及读

    }
}

// 接收下载文件数据
void TcpClient::receiveDownload(QDataStream &in){
    QFile file(m_downloadFilePath);
    // 发送确认命令字给客户端
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    QByteArray chunk;
    // 检查并以追加模式打开文件（如果未打开）
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Failed to open file for writing:" << file.fileName();
        return;
    }
    in >> chunk;
    file.write(chunk);
    m_downloadBytesReceived += chunk.size();
    qDebug() << m_downloadBytesReceived << m_downloadFileSize;
    if (m_downloadBytesReceived == m_downloadFileSize) {
        // 完整性检查
        file.close();
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        file.close();
        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();
        qDebug() << QString(fileHash);
        if (QString(fileHash) == m_downloadFileHash) {
            out << static_cast<ushort>(DOWNLOAD_COMPLETE);
        } else {
            qWarning("File hash mismatch");
        }
        m_socket->write(data); // 发送数据
        m_socket->flush(); // 刷新socket
    }

}

// 递归解析目录结构
void TcpClient::parseDirectory(QDataStream &in, QStandardItem *parentItem){
    int entryCount;
    in >> entryCount; // 读取当前目录中的文件和子目录数量

    for (int i = 0; i < entryCount; ++i) {
        QString fileName;
        bool isDir;
        in >> fileName >> isDir; // 读取文件或子目录名称和是否是目录

        QStandardItem *item = new QStandardItem(fileName);
        parentItem->appendRow(item); // 将文件或子目录添加到model中

        if (isDir) {
            parseDirectory(in, item); // 如果是目录，递归解析子目录结构
        }
    }
}


#include "tcpserver.h"
#include <QCryptographicHash>
#include <QThread>

TcpServer::TcpServer(QObject *parent)
    :QTcpServer(parent) {
    m_socket = new QTcpSocket(this);
    connect(this, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}


void TcpServer::onNewConnection() {
    m_socket = nextPendingConnection(); // 获取新连接的客户端socket
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead); // 设置读取数据信号和槽
}

// 处理客户端的请求
void TcpServer::onReadyRead() {
    m_socket = qobject_cast<QTcpSocket *>(sender());

    QByteArray data = m_socket->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    QString request;
    in >> request; // 读取客户端请求
    qDebug() << "收到请求:" << request;
    if (request == "GET_FILE_LIST") { // 如果请求为获取文件列表
        sendFileList(); // 发送文件列表
    }
    if (request == "GET_FILE_TREE") { // 如果请求为获取文件树
        sendFileTree(m_openPath); // 发送文件树
    }
    if (request == "REQUEST_UPLOAD_FILE") { // 客户端请求上传文件
        in >> m_uploadFileName >> m_uploadFileSize >> m_uploadFileHash;
        qDebug() << m_uploadFileName << m_uploadFileSize << m_uploadFileHash;

        responseUpload();
    }
    if (request == "UPLOAD_FILE") { // 客户端开始发送文件数据
        receiveUpload(in);
    }
    if (request == "REQUEST_DOWNLOAD_FILE") { // 客户端请求下载文件
        in >> m_downloadFileName;
        responseDownload();
    }
    if (request == "RECEIVE_FILE_READY") { //
        sendDownload();
    }
    if (request == "REQUEST_DELETE_FILE") { // 客户端请求删除文件
        QString fileName;
        in >> fileName;
        QString filePath = m_openPath.filePath(fileName);

        QFile file(filePath);
        if (file.exists()) {
            if (file.remove()) {
                qDebug() << "File deleted successfully:" << filePath;
            } else {
                qDebug() << "Failed to delete file:" << filePath;
            }
        } else {
            qDebug() << "File does not exist:" << filePath;
        }
    }

}

// 设置开放目录
void TcpServer::setOpenFolder(const QString &dir){
    m_openPath = QDir(dir); // 设置 openpath 成员变量, openpath 目录下不能有太多文件
}

// 发送文件列表给客户端
void TcpServer::sendFileList() {
    // QDir dir("/"); // 指定目录
    QDir dir(QDir::currentPath());

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot); // 获取文件列表

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("FILE_LIST"); // 指令字
    out << fileList.size(); // 先发送文件数量
    foreach (QFileInfo fileInfo, fileList) {
        out << fileInfo.fileName(); // 逐个发送文件名
    }

    m_socket->write(data); // 发送数据
    m_socket->flush(); // 刷新socket
}

// 发送文件树给客户端
void TcpServer::sendFileTree(const QDir &dir){
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out << QString("FILE_TREE"); // 指令字
    qDebug() << dir.absolutePath();
    out << dir.absolutePath();  // 发送根路径
    sendDirectory(out, dir);    // 递归发送目录结构

    m_socket->write(data); // 发送数据
    m_socket->flush(); // 刷新socket

}

// 递归发送目录结构
void TcpServer::sendDirectory(QDataStream &out, const QDir &dir){
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    out << entries.size(); // 发送当前目录中的文件和子目录数量
    foreach (QFileInfo entry, entries) {
        out << entry.fileName(); // 发送文件或子目录名称
        out << entry.isDir(); // 发送是否是目录
        if (entry.isDir()) {
            sendDirectory(out, QDir(entry.filePath())); // 如果是目录，递归发送子目录结构
        }
    }
}

// 回应文件上传请求
void TcpServer::responseUpload() {

    if (!m_openPath.exists()) { // 若 openPath 不存在，则在当前目录下创建 openPath
        m_openPath.mkpath(".");
    }

    QString filePath = m_openPath.filePath(m_uploadFileName);
    QFile file(filePath);

    // 发送确认命令字给客户端
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    if (file.open(QIODevice::WriteOnly)) { // 创建空文件
        file.close();
        out << QString("UPLOAD_FILE_READY");
    } else {
        qDebug() << "Failed to create file:" << file.errorString();
    }

    m_socket->write(data); // 发送数据
    m_socket->flush(); // 刷新socket
}

// 接收上传文件
void TcpServer::receiveUpload(QDataStream &in){
    QString filePath = m_openPath.filePath(m_uploadFileName);
    QFile file(filePath);
    qDebug() <<"Receive file in this path:" <<filePath;
    // 发送确认命令字给客户端
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    QByteArray chunk;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) { // 尝试以追加模式打开
        qWarning() << "Failed to open file for writing:" << file.fileName();
        return;
    }

    in >> chunk;
    file.write(chunk);
    m_uploadBytesReceived += chunk.size();
    qDebug() << m_uploadBytesReceived << m_uploadFileSize;
    if (m_uploadBytesReceived == m_uploadFileSize) {
        file.close();
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        file.close();
        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();
        qDebug() << QString(fileHash);
        if (QString(fileHash) == m_uploadFileHash) {
            out << QString("UPLOAD_COMPLETE");
        } else {
            qWarning("File hash mismatch");
        }
        m_socket->write(data); // 发送数据
        m_socket->flush(); // 刷新socket
    }

}

// 处理下载请求
void TcpServer::responseDownload(){
    QString fullPath = QDir(m_openPath).filePath(m_downloadFileName);
    QFileInfo fileInfo(fullPath);

    // 发送确认命令字给客户端
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    QFile file(fullPath);
    if (!file.exists()) {
        qDebug() << "File does not exist:" << fullPath;
        out << QString("DOWNLOAD_FILE_REJECT");
    }else{
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();

        m_downloadFilePath = fullPath;
        file.close();
        out << QString("DOWNLOAD_FILE_READY") << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
        qDebug() << QString("DOWNLOAD_FILE_READY") << fileInfo.fileName() << fileInfo.size() << QString(fileHash);

    }
    m_socket->write(data);
    m_socket->flush();
}

// 发送下载文件数据给客户端
void TcpServer::sendDownload(){
    QFile file(m_downloadFilePath);
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

        out << QString("DOWNLOAD_FILE") << chunk;
        qDebug() << count++ << "DOWNLOAD_FILE";
        bytesSent += chunk.size();

        m_socket->write(packet); // 将文件数据写入
        m_socket->waitForBytesWritten(3000); // 等待写完
        m_socket->flush(); // 刷新socket
        QThread::msleep(3); // 防止发送过快，客户端来不及读

    }
}





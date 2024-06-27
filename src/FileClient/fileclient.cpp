#include "fileclient.h"
#include "tcpclient.h"
#include <QFileInfo>
#include <QThread>
#include <QCryptographicHash>

FileClient::FileClient(TcpClient *tcpClient, QTreeView *view, QObject *parent)
    : QObject(parent), m_tcpClient(tcpClient), m_treeView(view) {

    model = new QStandardItemModel();
    m_treeView->setModel(model);
    m_uploadFile = new QFile(this);

    // 若连接成功，先请求文件树
    connect(tcpClient, &TcpClient::tcpConnectSuccess, this, &FileClient::requestFileTree);

    // 解析文件树
    connect(tcpClient, &TcpClient::fileTreeReceived, this, &FileClient::dealFileTree);

    // 处理文件列表
    connect(tcpClient, &TcpClient::fileListReceived, this, &FileClient::dealFileList);

    // 上传文件
    connect(tcpClient, &TcpClient::fileUploadReady, this, &FileClient::uploadFile);

    // 下载之前的准备
    connect(tcpClient, &TcpClient::fileDownloadReady, this, &FileClient::prepareForFileDownload);

    // 接收下载文件
    connect(tcpClient, &TcpClient::downloadFileReceived, this, &FileClient::receiveDownload);
}

// 请求文件列表
void FileClient::requestFileList() {
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

    out << static_cast<ushort>(GET_FILE_LIST); // 指令字 FILE_LIST
    m_tcpClient->enqueuePacket(request); // 发送请求

}

// 请求文件树
void FileClient::requestFileTree(){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);
    out << static_cast<ushort>(GET_FILE_TREE);
    m_tcpClient->enqueuePacket(request); // 发送请求
}

// 请求上传文件
void FileClient::requestUpload(const QString &filePath){
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

    qDebug() << "REQUEST_UPLOAD_FILE" << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    out << static_cast<ushort>(REQUEST_UPLOAD_FILE) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    m_tcpClient->enqueuePacket(request); // 发送请求


}

// 请求下载文件
void FileClient::requestDownload(const QString &fileName){
    m_downloadFileName = fileName; // 设置成员变量m_uploadFilePath

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    out << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    m_tcpClient->enqueuePacket(request); // 发送请求
}

// 请求删除文件
void FileClient::requestDelete(const QString &fileName){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << "REQUEST_DELETE_FILE" << fileName;
    out << static_cast<ushort>(REQUEST_DELETE_FILE) << fileName;
    m_tcpClient->enqueuePacket(request); // 发送请求
}

// 上传文件
void FileClient::uploadFile(){
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
        qDebug() << ++count << "UPLOAD_FILE";

        bytesSent += chunk.size();
        qDebug() << "已发送" << bytesSent;

        m_tcpClient->enqueuePacket(packet); // 将文件数据写入
        QThread::msleep(3); // 防止发送过快，服务器来不及读
    }
}

// 下载文件前的准备工作
void FileClient::prepareForFileDownload(QDataStream &in){
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

        m_tcpClient->enqueuePacket(request);
    }

}

// 接收下载文件数据
void FileClient::receiveDownload(QDataStream &in){
    int count = 0;
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

    count = ceil((double)m_downloadBytesReceived / 50000);
    qDebug() << count << m_downloadBytesReceived << m_downloadFileSize;
    if (m_downloadBytesReceived == m_downloadFileSize) {

        m_downloadBytesReceived = 0;

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
        m_tcpClient->enqueuePacket(data);
    }

}

// 解析文件树
void FileClient::dealFileTree(QDataStream &in){
    QString rootPath;
    in >> rootPath; // 读取根路径

    model->clear(); // 清除现有的model数据
    QStandardItem *rootItem = new QStandardItem(rootPath);
    model->appendRow(rootItem); // 将根路径添加到model中

    parseDirectory(in, rootItem); // 解析目录结构
}

// 处理文件列表
void FileClient::dealFileList(QDataStream &in){
    int fileCount;
    in >> fileCount; // 读取文件数量

    model->clear(); // 清除现有的model数据
    for (int i = 0; i < fileCount; ++i) {
        QString fileName;
        in >> fileName; // 读取每个文件名
        QStandardItem *item = new QStandardItem(fileName);
        model->appendRow(item); // 将文件名添加到model中
    }
}


// 递归解析目录结构
void FileClient::parseDirectory(QDataStream &in, QStandardItem *parentItem){
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



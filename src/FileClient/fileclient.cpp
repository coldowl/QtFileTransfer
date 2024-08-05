#include <QFileInfo>
#include <QThread>
#include <QTimer>
#include <QCryptographicHash>
#include "fileclient.h"
#include "tcpclient.h"
#include "fileclient.h"

FileClient::FileClient(QObject *parent)
    : QObject(parent){

}


// 初始化函数
void FileClient::init(){
    m_model = new QStandardItemModel(this);
    m_tcpClient = new TcpClient(this);
    m_uploadFile = new QFile(this);

}

// 请求文件列表
void FileClient::requestFileList() {
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

    out << static_cast<ushort>(GET_FILE_LIST); // 指令字 FILE_LIST

    emit readyForWrap(request);

}

// 请求文件树
void FileClient::requestFileTree(){
    // qDebug() << "正在执行requestFileTree";
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);
    out << static_cast<ushort>(GET_FILE_TREE);
    // qDebug() << "发送信号readyForWrap";
    emit readyForWrap(request);
}


// 请求上传文件
void FileClient::requestUpload(const QString &filePath){
    m_uploadFilePath = filePath;
    QFile file(filePath);
    QFileInfo fileInfo(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("打开文件失败！");
        return;
    }

    QByteArray fileData = file.readAll();
    QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    // qDebug() << "REQUEST_UPLOAD_FILE" << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    out << static_cast<ushort>(REQUEST_UPLOAD_FILE) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);

    emit readyForWrap(request);

    // 发射信号给传输进度窗口
    QByteArray basicInfo;
    QDataStream outInfo(&basicInfo, QIODevice::WriteOnly);
    outInfo << fileInfo.fileName() << fileInfo.size();
    emit uploadedFileInfo(basicInfo);

}

// 请求下载文件
void FileClient::requestDownload(const QString &fileName){
    m_downloadFileName = fileName; // 设置成员变量m_uploadFilePath

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    // qDebug() << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    out << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    emit readyForWrap(request);
}

// 请求删除文件
void FileClient::requestDelete(const QString &fileName){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    // qDebug() << "REQUEST_DELETE_FILE" << fileName;
    out << static_cast<ushort>(REQUEST_DELETE_FILE) << fileName;
    emit readyForWrap(request);
}


// 上传文件
void FileClient::uploadFile(){
    QFile file(m_uploadFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "打不开" << m_uploadFilePath;
        return;
    }

    QByteArray fileData = file.readAll();

    qint64 bytesSent = 0;
    int count = 0;
    int totalSize = fileData.size();
    m_timer.restart();

    while (bytesSent < fileData.size()) {
        QByteArray packet;
        QDataStream out(&packet,QIODevice::WriteOnly);
        QByteArray chunk = fileData.mid(bytesSent, 50000);

        out << static_cast<ushort>(UPLOAD_FILE) << chunk;
        // qDebug() << ++count << "UPLOAD_FILE";

        bytesSent += chunk.size();
        emit bytesAlreadySent(bytesSent); // 给文件传输窗口

        emit timeStarted(m_timer);

        emit readyForWrap(packet);
    }

}

// 下载文件前的准备工作
void FileClient::prepareForFileDownload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    in >> m_downloadFileName >> m_downloadFileSize >> m_downloadFileHash;
    // qDebug() << m_downloadFileName << m_downloadFileSize << m_downloadFileHash;

    // 发射信号给传输进度窗口
    QByteArray basicInfo;
    QDataStream outInfo(&basicInfo, QIODevice::WriteOnly);
    outInfo << m_downloadFileName << m_downloadFileSize;
    emit downloadedFileInfo(basicInfo);

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

        // qDebug() << "send: RECEIVE_FILE_READY";
        out << static_cast<ushort>(RECEIVE_FILE_READY); // 指令字 RECEIVE_FILE_READY

        emit readyForWrap(request);
    }

}

// 接收下载文件数据
void FileClient::receiveDownload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    int count = 0;
    QFile file(m_downloadFilePath);

    // 发送确认命令字给客户端
    QByteArray command;
    QDataStream out(&command, QIODevice::WriteOnly);

    QByteArray chunk;
    // 检查并以追加模式打开文件（如果未打开）
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Failed to open file for writing:" << file.fileName();
        return;
    }

    in >> chunk;
    file.write(chunk);
    m_downloadBytesReceived += chunk.size();
    emit bytesAlreadyRcv(m_downloadBytesReceived); // 给文件传输窗口

    count = ceil((double)m_downloadBytesReceived / 50000);
    // qDebug() << count << m_downloadBytesReceived << m_downloadFileSize;
    if (m_downloadBytesReceived == m_downloadFileSize) {

        m_downloadBytesReceived = 0;

        // 完整性检查
        file.close();
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        file.close();
        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();
        // qDebug() << QString(fileHash);
        if (QString(fileHash) == m_downloadFileHash) {
            out << static_cast<ushort>(DOWNLOAD_COMPLETE);
        } else {
            qWarning("文件不匹配");
        }
        emit readyForWrap(command);
    }

}

// 解析文件树
void FileClient::dealFileTree(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    QString rootPath;
    in >> rootPath; // 读取根路径

    m_model->clear(); // 清除现有的model数据

    m_model->setHorizontalHeaderLabels({"文件名", "操作1", "操作2"}); // 设置表头

    QStandardItem *rootItem = new QStandardItem(rootPath);
    m_model->appendRow(rootItem); // 将根路径添加到model中

    parseDirectory(in, rootItem); // 解析目录结构

    emit fileTreeModel(m_model); // 发送解析完成的文件树
}

// 处理文件列表
void FileClient::dealFileList(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    int fileCount;
    in >> fileCount; // 读取文件数量

    m_model->clear(); // 清除现有的model数据

    for (int i = 0; i < fileCount; ++i) {
        QString fileName;
        in >> fileName; // 读取每个文件名
        QStandardItem *fileItem = new QStandardItem(fileName);
        m_model->appendRow(fileItem); // 将文件名添加到model中
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

        QStandardItem *fileItem = new QStandardItem(fileName);
        QStandardItem *deleteItem = new QStandardItem("[删除]");
        // deleteItem->setIcon(QIcon(":/icons/delete.png"));
        QStandardItem *downloadItem = new QStandardItem("[下载]");
        // downloadItem->setIcon(QIcon(":/icons/download.png"));

        parentItem->appendRow({fileItem, downloadItem, deleteItem}); // 将文件或子目录添加到model中

        if (isDir) {
            parseDirectory(in, fileItem); // 如果是目录，递归解析子目录结构
        }
    }
}



#include "fileserver.h"
#include <QCryptographicHash>
#include <QThread>

FileServer::FileServer(QObject *parent)
    : QObject(parent){

}

// 设置开放目录
void FileServer::setOpenFolder(const QString &dir){
    m_openPath = QDir(dir); // 设置 openpath 成员变量, openpath 目录下不能有太多文件
}

// 发送文件列表给客户端
void FileServer::sendFileList(){
    QDir dir(QDir::currentPath());

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot); // 获取文件列表

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << static_cast<ushort>(FILE_LIST);
    out << fileList.size(); // 先发送文件数量
    foreach (QFileInfo fileInfo, fileList) {
        out << fileInfo.fileName(); // 逐个发送文件名
    }

    emit readyForWrap(data);
    // m_tcpServer->enqueuePacket(data);
}

// 发送文件树给客户端
void FileServer::sendFileTree(){
    // qDebug() << "正在执行sendFileTree";
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out << static_cast<ushort>(FILE_TREE);
    qDebug() << "m_openPath.absolutePath" << m_openPath.absolutePath();
    out <<  m_openPath.absolutePath();
    sendDirectory(out, m_openPath);    // 递归发送目录结构

    emit readyForWrap(data);
    // qDebug() << "发射信号readyForWrap";
    // m_tcpServer->enqueuePacket(data);
}

// 递归发送目录结构
void FileServer::sendDirectory(QDataStream &out, const QDir &dir){
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
void FileServer::responseUpload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    in >> m_uploadFileName >> m_uploadFileSize >> m_uploadFileHash;
    qDebug() << "上传文件名" <<m_uploadFileName << "文件大小" << m_uploadFileSize << "哈希" << m_uploadFileHash;
    m_uploadBytesReceived = 0;

    if (!m_openPath.exists()) { // 若 openPath 不存在，则在当前目录下创建 openPath
        m_openPath.mkpath(".");
    }

    QString filePath = m_openPath.filePath(m_uploadFileName);
    QFile file(filePath);

    // 发送确认命令字给客户端
    QByteArray command;
    QDataStream out(&command, QIODevice::WriteOnly);

    if (file.open(QIODevice::WriteOnly)) { // 创建空文件
        file.close();
        out << static_cast<ushort>(UPLOAD_FILE_READY);
    } else {
        qDebug() << "Failed to create file:" << file.errorString();
    }

    emit readyForWrap(command);
    // m_tcpServer->enqueuePacket(data);
}

// 接收上传文件
void FileServer::receiveUpload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    int count = 0;
    QString filePath = m_openPath.filePath(m_uploadFileName);
    QFile file(filePath);
    // qDebug() <<"Receive file in this path:" <<filePath;

    // 发送确认命令字给客户端
    QByteArray command;
    QDataStream out(&command, QIODevice::WriteOnly);

    QByteArray chunk;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) { // 尝试以追加模式打开
        qWarning() << "Failed to open file for writing:" << file.fileName();
        return;
    }

    in >> chunk;
    file.write(chunk);
    m_uploadBytesReceived += chunk.size();

    count = ceil((double)m_uploadBytesReceived / 50000);
    // qDebug() << m_uploadBytesReceived << m_uploadFileSize;
    if (m_uploadBytesReceived == m_uploadFileSize) {

        m_uploadBytesReceived = 0;

        file.close();
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        file.close();

        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();

        if (QString(fileHash) == m_uploadFileHash) {
            out << static_cast<ushort>(UPLOAD_COMPLETE);
        } else {
            qWarning("File hash mismatch");
        }

        emit readyForWrap(command);
        // m_tcpServer->enqueuePacket(data);
    }

}

// 处理下载请求
void FileServer::responseDownload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    in >> m_downloadFileName;

    QString fullPath = QDir(m_openPath).filePath(m_downloadFileName);
    QFileInfo fileInfo(fullPath);

    // 发送确认命令字给客户端
    QByteArray command;
    QDataStream out(&command, QIODevice::WriteOnly);

    QFile file(fullPath);
    if (!file.exists()) {
        qDebug() << "File does not exist:" << fullPath;
        out << static_cast<ushort>(DOWNLOAD_FILE_REJECT);
    }else{
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll();
        QByteArray fileHash = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();

        m_downloadFilePath = fullPath;
        file.close();
        out << static_cast<ushort>(DOWNLOAD_FILE_READY) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
        // qDebug() << static_cast<ushort>(DOWNLOAD_FILE_READY) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);

    }

    emit readyForWrap(command);
    // m_tcpServer->enqueuePacket(data);
}

// 发送下载文件数据给客户端
void FileServer::sendDownload(){
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

        out << static_cast<ushort>(DOWNLOAD_FILE) << chunk;
        // qDebug() << ++count << "DOWNLOAD_FILE";

        bytesSent += chunk.size();
        // qDebug() << "已发送" << bytesSent;

        emit readyForWrap(packet);
        // m_tcpServer->enqueuePacket(packet);

        // QThread::msleep(3); // 防止发送过快，服务器来不及读

    }
}

void FileServer::deleteFile(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    QString fileName;
    in >> fileName;
    QString filePath = m_openPath.filePath(fileName);

    QFile file(filePath);
    if (file.exists()) {
        if (file.remove()) {
            qDebug() << "成功删除文件:" << filePath;
        } else {
            qDebug() << "删除:" << filePath << "失败！";
        }
    } else {
        qDebug() << "文件不存在:" << filePath;
    }
}

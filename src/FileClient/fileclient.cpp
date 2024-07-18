#include "fileclient.h"
#include "tcpclient.h"
#include "fileclient.h"
#include <QFileInfo>
#include <QThread>
#include <QTimer>
// #include <QDataStream>
// #include <QRunnable>
#include <QCryptographicHash>

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
    qDebug() << "正在执行requestFileTree";
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);
    out << static_cast<ushort>(GET_FILE_TREE);
    qDebug() << "发送信号readyForWrap";
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

    qDebug() << "REQUEST_UPLOAD_FILE" << fileInfo.fileName() << fileInfo.size() << QString(fileHash);
    out << static_cast<ushort>(REQUEST_UPLOAD_FILE) << fileInfo.fileName() << fileInfo.size() << QString(fileHash);

    QByteArray basicInfo;
    QDataStream outInfo(&basicInfo, QIODevice::WriteOnly);
    outInfo << fileInfo.fileName() << fileInfo.size();
    basicInfo.append("准备上传");
    emit uploadBasicInfo(basicInfo); // 发射信号给传输进度窗口
    emit progressUpdated(0); // 确保发送前进度为0


    emit readyForWrap(request);


}

// 请求下载文件
void FileClient::requestDownload(const QString &fileName){
    m_downloadFileName = fileName; // 设置成员变量m_uploadFilePath

    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    out << static_cast<ushort>(REQUEST_DOWNLOAD_FILE) << fileName;
    emit readyForWrap(request);
}

// 请求删除文件
void FileClient::requestDelete(const QString &fileName){
    QByteArray request;
    QDataStream out(&request,QIODevice::WriteOnly);

    qDebug() << "REQUEST_DELETE_FILE" << fileName;
    out << static_cast<ushort>(REQUEST_DELETE_FILE) << fileName;
    emit readyForWrap(request);
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
    int totalSize = fileData.size();

    QTimer timer;
    connect(&timer, &QTimer::timeout, [this, &bytesSent, totalSize]() {
        int progress = static_cast<int>((bytesSent * 100) / totalSize);
        emit progressUpdated(progress);
    });
    timer.start(100); // 每100毫秒发送一次进度信号

    while (bytesSent < fileData.size()) {
        QByteArray packet;
        QDataStream out(&packet,QIODevice::WriteOnly);
        QByteArray chunk = fileData.mid(bytesSent, 50000);

        out << static_cast<ushort>(UPLOAD_FILE) << chunk;
        qDebug() << ++count << "UPLOAD_FILE";

        bytesSent += chunk.size();
        qDebug() << "已发送" << bytesSent;

        // QByteArray progressInfo;
        // QDataStream outInfo(&progressInfo, QIODevice::WriteOnly);
        // // qint64 test1 = 85;
        // // qint64 test2 = 100;
        // // outInfo << test1 << test2;
        // // progressInfo.append(bytesSent+fileData.size());
        // outInfo << bytesSent << static_cast<qint64>(fileData.size());
        // emit uploadProgressInfo(progressInfo); // 给文件传输窗口
        // qDebug() << "已发射信号";

        emit readyForWrap(packet);

        QThread::msleep(30); // 防止发送过快，服务器来不及读
    }
    timer.stop(); // 停止计时器

    emit progressUpdated(100); // 确保发送最后的进度信号

}

// 下载文件前的准备工作
void FileClient::prepareForFileDownload(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
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
        emit readyForWrap(command);
    }

}

// 解析文件树
void FileClient::dealFileTree(QByteArray data){
    QDataStream in(&data, QIODevice::ReadOnly);
    QString rootPath;
    in >> rootPath; // 读取根路径

    m_model->clear(); // 清除现有的model数据
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
        QStandardItem *item = new QStandardItem(fileName);
        m_model->appendRow(item); // 将文件名添加到model中
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



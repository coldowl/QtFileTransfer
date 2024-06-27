#ifndef FILESERVER_H
#define FILESERVER_H

#include "tcpserver.h"
#include <QDir>
#include <QFileInfoList>
#include <QDataStream>

//服务器发出的指令0x1开头
#define FILE_TREE 0x1001
#define FILE_LIST 0x1002
#define UPLOAD_FILE_READY 0x1003
#define UPLOAD_COMPLETE 0x1004
#define DOWNLOAD_FILE_READY 0x1005
#define DOWNLOAD_FILE 0x1006
#define RECEIVE_FILE_READY 0x1007

//客户端发出的指令0x2开头
#define GET_FILE_TREE 0x2001
#define GET_FILE_LIST 0x2002
#define REQUEST_UPLOAD_FILE 0x2003
#define REQUEST_DOWNLOAD_FILE 0x2004
#define REQUEST_DELETE_FILE 0x2005
#define UPLOAD_FILE 0x2006
#define DOWNLOAD_COMPLETE 0x2007
#define DOWNLOAD_FILE_REJECT 0x2008

class FileServer : public QObject{
    Q_OBJECT

public:
    explicit FileServer(TcpServer *tcpServer, QObject *parent = nullptr);

    // 自定义客户端可操作的文件夹
    void setOpenFolder(const QString &dir);


private slots:
    // 发送文件列表给客户端
    void sendFileList();

    // 发送文件树给客户端
    void sendFileTree();

    // 发送文件目录给客户端
    void sendDirectory(QDataStream &out, const QDir &dir);

    // 处理文件上传请求
    void responseUpload(QDataStream &in);

    // 接收上传文件
    void receiveUpload(QDataStream &in);

    // 处理文件下载请求
    void responseDownload(QDataStream &in);

    // 发送下载文件
    void sendDownload();

    // 删除文件
    void deleteFile(QDataStream &in);

private:
    TcpServer *m_tcpServer = nullptr;
    QDir m_openPath; // 开放文件夹给客户端

    // 上传文件的文件信息
    QString m_uploadFileName = "";
    qint64 m_uploadFileSize = 0;
    qint64 m_uploadBytesReceived = 0;
    QString m_uploadFileHash = "";

    // 下载文件的文件信息
    QString m_downloadFileName = "";
    QString m_downloadFilePath = "";

};

#endif // FILESERVER_H

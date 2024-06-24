#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QFileInfoList>
#include <QDataStream>

// const int DATA_BLOCK_SIZE = 1024;
// const quint16 REQUEST_UPLOAD_FILE = 0x0001;
// const quint16 UPLOAD_FILE_OK = 0x0002;
// const quint16 UPLOAD_FILE = 0x0003;
// const quint16 UPLOAD_COMPLETE = 0x0004;

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

// 服务器类继承自QTcpServer
class TcpServer : public QTcpServer {
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);

    // 自定义客户端可操作的文件夹
    void setOpenFolder(const QString &dir);


private slots:
    // 处理新连接
    void onNewConnection();

    // 处理客户端的请求
    void onReadyRead();

    // 发送文件列表给客户端
    void sendFileList();

    // 发送文件树给客户端
    void sendFileTree(const QDir &dir);

    // 发送文件目录给客户端
    void sendDirectory(QDataStream &out, const QDir &dir);

    // 处理文件上传请求
    void responseUpload();

    // 接收上传文件
    void receiveUpload(QDataStream &in);

    // 处理文件下载请求
    void responseDownload();

    // 发送下载文件
    void sendDownload();

private:
    QDir m_openPath; // 开放文件夹给客户端
    QTcpSocket *m_socket = nullptr;

    // 上传文件的文件信息
    QString m_uploadFileName = "";
    qint64 m_uploadFileSize = 0;
    qint64 m_uploadBytesReceived = 0;
    QString m_uploadFileHash = "";

    // 下载文件的文件信息
    QString m_downloadFileName = "";
    QString m_downloadFilePath = "";
};


#endif // TCPSERVER_H

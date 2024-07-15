#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QFileInfoList>
#include <QDataStream>
#include <QQueue>

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

    // // 自定义客户端可操作的文件夹
    // void setOpenFolder(const QString &dir);

    // 发数据
    void enqueuePacket(const QByteArray &packet);

signals:
    // // 获取文件列表
    // void getFileList();

    // // 获取文件树
    // void getFileTree();

    // // 请求上传文件
    // void requestUploadFile(QDataStream &in);

    // // 接收上传文件
    // void uploadFileReceived(QDataStream &in);

    // // 请求下载文件
    // void requestDownloadFile(QDataStream &in);

    // // 客户端准备好接收文件
    // void receiveFileReady();

    // // 请求删除文件
    // void requestDeleteFile(QDataStream &in);

    // 准备好解协议
    void readyForProtocolParse(const QByteArray &protocolPacket);

private slots:
    // 处理新连接
    void onNewConnection();

    // 处理客户端的请求
    void onReadyRead();



private:

    QTcpSocket *m_socket = nullptr;
    QQueue<QByteArray> m_packetQueue;

    bool m_isSending = false;

};


#endif // TCPSERVER_H

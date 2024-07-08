#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QDir>
#include <QQueue>

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


class TcpClient : public QObject {
    Q_OBJECT

public:
    // 构造函数
    explicit TcpClient(QObject *parent = nullptr);

    // 连接到服务器
    bool connectToServer(const QString &host, const quint16 port);

    // 断开连接
    bool disconnectFromServer();

    // 将代发数据包加入队列
    void enqueuePacket(const QByteArray &packet);


signals:
    // 文件树
    void fileTreeReceived(QDataStream &in);

    // 文件列表
    void fileListReceived(QDataStream &in);

    // 准备上传文件
    void fileUploadReady();

    // 文件上传成功,有待完善应该携带文件名
    void fileUploadSuccess();

    // 准备下载文件
    void fileDownloadReady(QDataStream &in);

    // 下载文件
    void downloadFileReceived(QDataStream &in);

    // TCP建立连接成功
    void tcpConnectSuccess();

    // 准备好解协议
    void readyForProtocolParse(QDataStream &in);


private slots:
    // 读取服务器发送的数据
    void onReadyRead();

    // 发送报文
    void sendNextPacket();

private:

    QTcpSocket *m_socket = nullptr;
    QQueue<QByteArray> m_packetQueue;

    bool m_isSending = false;

};

#endif //TCPCLIENT_H

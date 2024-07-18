#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QDir>
#include <QQueue>

class TcpClient : public QObject {
    Q_OBJECT

public:
    // 构造函数
    explicit TcpClient(QObject *parent = nullptr);

    // 连接到服务器
    void connectToServer(const QString &host, const quint16 port);

    // 断开连接
    void disconnectFromServer();

    // 将代发数据包加入队列
    void enqueuePacket(const QByteArray &packet);

    // 初始化函数
    void init();

signals:

    // TCP建立连接成功
    void tcpConnectSuccess();

    // TCP建立连接失败
    void tcpConnectFailed();

    // 断开TCP连接成功
    void tcpDisconnectSuccess();

    // 断开TCP连接失败
    void tcpDisconnectFailed();

    // 准备好解协议
    void readyForProtocolParse(const QByteArray &data);

    // 用于发送 socket 状态改变的通知
    void stateChanged(QString stateString);


private slots:
    // 读取服务器发送的数据
    void onReadyRead();

    // 套接字状态改变
    void onStateChanged(QAbstractSocket::SocketState state);


private:

    QTcpSocket *m_socket = nullptr;
    // QQueue<QByteArray> m_packetQueue;
    // bool m_isSending = false;

};

#endif //TCPCLIENT_H

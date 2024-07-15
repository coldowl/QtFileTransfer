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
    bool connectToServer(const QString &host, const quint16 port);

    // 断开连接
    bool disconnectFromServer();

    // 将代发数据包加入队列
    void enqueuePacket(const QByteArray &packet);


signals:

    // TCP建立连接成功
    void tcpConnectSuccess();

    // 准备好解协议
    void readyForProtocolParse(const QByteArray &data);


private slots:
    // 读取服务器发送的数据
    void onReadyRead();

    // 发送报文
    void sendNextPacket();

private:

    QTcpSocket *m_socket = nullptr;
    // QQueue<QByteArray> m_packetQueue;
    // bool m_isSending = false;

};

#endif //TCPCLIENT_H

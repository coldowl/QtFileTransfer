#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>

class UdpClient : public QObject
{
    Q_OBJECT

public:
    explicit UdpClient(QObject *parent = nullptr);

    // 发送数据报
    void sendDatagram(const QByteArray &datagram);

    // 设置服务器ip和端口号
    void setConnectInfo(const QString serverIp, qint16 serverPort);

    // 关闭 udp 套接字
    void closeUdpSocket();

    // 初始化函数
    void init();

signals:
    // 设置连接成功
    void setUdpConnectInfoSuccess();

    // 设置连接失败
    void setUdpConnectInfoFailed();

    // 关闭套接字成功
    void udpCloseSuccess();

    // 关闭套接字失败
    void udpCloseFailed();

    // 准备好解析协议
    void readyForProtocolParse(const QByteArray &data);

private slots:
    // 读数据报
    void onReadyRead();

private:
    QUdpSocket *m_udpSocket = nullptr;

    QString m_serverIp = "";
    qint16 m_serverPort = 0;
};

#endif // UDPCLIENT_H

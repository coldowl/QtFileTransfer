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
    bool setConnectInfo(const QString serverIp, qint16 serverPort);

    // 关闭 udp 套接字
    bool closeUdpSocket();

signals:
    void setUdpConnectInfoSuccess();

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

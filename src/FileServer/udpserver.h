#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);

    // 发送数据报
    void sendDatagram(const QByteArray &datagram);

signals:
    // 准备好解协议
    void readyForProtocolParse(const QByteArray &protocolPacket);

private slots:
    // 读数据报
    void onReadyRead();

private:
    QUdpSocket *udpSocket;

    QHostAddress m_senderAddress;
    quint16 m_senderPort = 0;
};

#endif // UDPSERVER_H

#include "udpserver.h"
#include <QNetworkDatagram>
#include <QHostAddress>

UdpServer::UdpServer(QObject *parent) : QObject(parent){

    udpSocket = new QUdpSocket(this);

    // 只监听指定端口的UDP报文
    if (udpSocket->bind(QHostAddress::Any, 5555)) {
        connect(udpSocket, &QUdpSocket::readyRead, this, &UdpServer::onReadyRead);
    } else {
        qDebug() << "Failed to bind to port";
    }
}

// 发送数据报
void UdpServer::sendDatagram(const QByteArray &datagram){

    udpSocket->writeDatagram(datagram, m_senderAddress, m_senderPort);
}


// 接收数据报
void UdpServer::onReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray data = datagram.data();
        m_senderAddress = datagram.senderAddress();
        m_senderPort = datagram.senderPort();

        // 处理接收到的数据
        qDebug() << "客户端地址" << m_senderAddress.toString() << ":" << m_senderPort << data;
        // 解协议
        emit readyForProtocolParse(data);
    }
}

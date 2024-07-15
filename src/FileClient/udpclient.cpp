#include "UdpClient.h"

UdpClient::UdpClient(QObject *parent) : QObject(parent){

    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpClient::onReadyRead);
}


// 发送数据报
void UdpClient::sendDatagram(const QByteArray &datagram){

    m_udpSocket->writeDatagram(datagram, QHostAddress(m_serverIp), m_serverPort);
}

// 接收数据报
void UdpClient::onReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        emit readyForProtocolParse(datagram);
        qDebug() << "收到UDP报文：" << QString::fromUtf8(datagram.data(), datagram.size());
    }
}

// 设置数据报目的IP和端口号
bool UdpClient::setConnectInfo(const QString serverIp, qint16 serverPort){
    if(serverIp == "" || serverPort == 0){
        return false;
    }
    m_serverIp = serverIp;
    m_serverPort = serverPort;
    emit setUdpConnectInfoSuccess(); // 设置连接成功后发送信号
    return true;
}

// 关闭 UDP 套接字
bool UdpClient::closeUdpSocket(){
    if (m_udpSocket) {
        m_udpSocket->close();
        delete m_udpSocket;
        m_udpSocket = nullptr;
        return true; // 关闭成功
    }
    return false; // udpSocket 为空，关闭失败
}

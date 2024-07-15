#include "tcpclient.h"
#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QThread>
#include <QtEndian>


// 构造函数，初始化treeView和model
TcpClient::TcpClient(QObject *parent)
    : QObject(parent){

    m_socket = new QTcpSocket(this); // 初始化 TCP socket

    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead); // 接收并处理报文
}

// 连接到服务器
bool TcpClient::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port); // 连接到指定的host和端口
    if (m_socket->waitForConnected()) { // 等待连接
        qDebug() << "发送信号tcpConnectSuccess";
        emit tcpConnectSuccess(); // 连接成功后发送信号
        return true;
    }else{
        return false;
    }
}

// 断开和服务器的连接
bool TcpClient::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            if (!m_socket->waitForDisconnected()) {
                qDebug() << "Error disconnecting:" << m_socket->errorString();
                return false;
            }
        }
    }
    return m_socket->state() == QAbstractSocket::UnconnectedState; // 返回 1 表示断开连接成功
}

// 读取服务器发送的数据
void TcpClient::onReadyRead() {

    QByteArray data = m_socket->readAll();

    // qDebug() << "收到协议包：" << data.toHex();

    // 解协议
    emit readyForProtocolParse(data);
    qDebug() << "发射readyForProtocolParse";

}

// 报文入队
void TcpClient::enqueuePacket(const QByteArray &packet) {
    qDebug() << "正在执行enqueuePacket";
    // qDebug() << "协议包：" << packet.toHex();
    m_socket->write(packet); // 将文件数据写入
    // m_socket->flush(); // 刷新socket

    // m_packetQueue.enqueue(packet); // 报文入队
    // if (!m_isSending) {
    //     m_isSending = true;
    //     sendNextPacket();
    // }
}

// 发送报文
void TcpClient::sendNextPacket() {
    // if (!m_packetQueue.isEmpty()) {
        // QByteArray packet = m_packetQueue.dequeue(); // 报文出队
        // m_socket->write(packet); // 将文件数据写入
        // m_socket->flush(); // 刷新socket
    // } else {
    //     m_isSending = false;
    //     qDebug() << "All packets sent";
    // }
}




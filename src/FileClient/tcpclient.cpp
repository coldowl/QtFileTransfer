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

    // m_socket = new QTcpSocket(this); // 初始化 TCP socket

    // connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead); // 接收并处理报文
}


// 初始化函数
void TcpClient::init() {
    m_socket = new QTcpSocket(this); // 在新的线程中初始化 TCP socket
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead); // 接收并处理报文
    connect(m_socket, &QAbstractSocket::stateChanged, this, &TcpClient::onStateChanged); // 当套接字状态改变时，发射它的状态
}

// 连接到服务器
void TcpClient::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port); // 连接到指定的host和端口
    if (m_socket->waitForConnected()) { // 等待连接
        qDebug() << "发送信号tcpConnectSuccess";
        emit tcpConnectSuccess(); // 连接成功后发送信号
    }else{
        emit tcpConnectFailed(); // 连接失败
    }
}

// 断开和服务器的连接
void TcpClient::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            if (!m_socket->waitForDisconnected()) {
                qDebug() << "Error disconnecting:" << m_socket->errorString();
                emit tcpDisconnectFailed(); // 断开TCP连接失败
            }
        }
    }
    emit tcpDisconnectSuccess(); // 断开TCP连接成功
}

// 读取服务器发送的数据
void TcpClient::onReadyRead() {

    QByteArray data = m_socket->readAll();

    // qDebug() << "收到协议包：" << data.toHex();

    // 解协议
    emit readyForProtocolParse(data);
    qDebug() << "发射readyForProtocolParse";

}

void TcpClient::onStateChanged(QAbstractSocket::SocketState state){
    QString stateString;
    // 根据 socket 的状态执行相应的操作
    switch (state)
    {
    case QAbstractSocket::ConnectedState :
        stateString = "TCP Socket 已连接";
        break;
    case QAbstractSocket::UnconnectedState:
        stateString = "TCP Socket 已断开连接";
        break;
    case QAbstractSocket::HostLookupState:
        stateString = "正在进行主机名解析";
        break;
    case QAbstractSocket::ConnectingState:
        stateString = "正在尝试连接";
        break;
    case QAbstractSocket::BoundState:
        stateString = "TCP Socket 已绑定到地址和端口";
        break;
    case QAbstractSocket::ClosingState:
        stateString = "TCP Socket 正在关闭";
        break;
    default:
        stateString = "未知的 Socket 状态";
        break;
    }

    emit stateChanged(stateString); // 发送信号，通知 socket 状态已改变
    qDebug() << "发送socket状态" << stateString;
}

// 发送报文
void TcpClient::enqueuePacket(const QByteArray &packet) {
    qDebug() << "正在执行enqueuePacket";
    // qDebug() << "协议包：" << packet.toHex();
    m_socket->write(packet); // 将文件数据写入
}






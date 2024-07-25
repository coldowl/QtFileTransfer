#include "tcpserver.h"
#include <QCryptographicHash>
#include <QThread>

TcpServer::TcpServer(QObject *parent)
    :QTcpServer(parent) {
    m_socket = new QTcpSocket(this);
    connect(this, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

void TcpServer::onNewConnection() {
    m_socket = nextPendingConnection(); // 获取新连接的客户端socket
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead); // 设置读取数据信号和槽
    connect(m_socket, &QAbstractSocket::stateChanged, this, &TcpServer::onStateChanged); // 当套接字状态改变时，发射它的状态
}

// 处理客户端的请求
void TcpServer::onReadyRead() {
    m_socket = qobject_cast<QTcpSocket *>(sender());

    QByteArray data = m_socket->readAll();

    // qDebug() << "收到协议包：" << data.toHex();

    // 解协议
    emit readyForProtocolParse(data);
    qDebug() << "发射readyForProtocolParse";

}

void TcpServer::onStateChanged(QAbstractSocket::SocketState state){
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
        stateString = "正在进行主机名解析...";
        break;
    case QAbstractSocket::ConnectingState:
        stateString = "正在尝试连接...";
        break;
    case QAbstractSocket::BoundState:
        stateString = "TCP Socket 已绑定到地址和端口";
        break;
    case QAbstractSocket::ClosingState:
        stateString = "TCP Socket 正在关闭";
        break;
    case QAbstractSocket::ListeningState:
        stateString = "正在监听中...";
        break;
    default:
        stateString = "未知的 Socket 状态";
        break;
    }

    emit stateChanged(stateString); // 发送信号，通知 socket 状态已改变
    qDebug() << "发送socket状态" << stateString;
}


// 发送报文
void TcpServer::enqueuePacket(const QByteArray &packet) {
    // qDebug() << "正在执行enqueuePacket";
    // qDebug() << "发送协议包：" << packet.toHex();
    m_socket->write(packet); // 将文件数据写入
}





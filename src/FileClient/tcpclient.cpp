#include "tcpclient.h"
#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QThread>


// 构造函数，初始化treeView和model
TcpClient::TcpClient(QObject *parent)
    : QObject(parent){

    m_socket = new QTcpSocket(this); // 初始化 TCP socket

    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead); // 接收并处理报文
    // connect(m_socket, &QTcpSocket::bytesWritten, this, &TcpClient::sendNextPacket); // 写入并发送报文
}

// 连接到服务器
bool TcpClient::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port); // 连接到指定的host和端口
    if (m_socket->waitForConnected()) { // 等待连接

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
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15); // 确保数据流版本匹配

    ushort command;
    in >> command; // 读取指令字
    qDebug() << "收到指令字:" << Qt::hex << Qt::showbase << command;

    switch(command) {
    case FILE_TREE: { // 文件树
        emit fileTreeReceived(in);
        break;
    }
    case FILE_LIST: { // 文件列表
        emit fileListReceived(in);
        break;
    }
    case UPLOAD_FILE_READY: { // 准备上传文件
        emit fileUploadReady();
        break;
    }
    case UPLOAD_COMPLETE: { // 上传完成
        // 上传完成后的处理
        break;
    }
    case DOWNLOAD_FILE_READY: { // 准备下载文件
        emit fileDownloadReady(in);
        break;
    }
    case DOWNLOAD_FILE: { // 下载文件
        emit downloadFileReceived(in);
        break;
    }
    default:{ // 其他指令类型处理
        qWarning() << "未知指令字:" << command;
        break;
    }
    }
}

// 报文入队
void TcpClient::enqueuePacket(const QByteArray &packet) {
    m_socket->write(packet); // 将文件数据写入
    m_socket->flush(); // 刷新socket
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



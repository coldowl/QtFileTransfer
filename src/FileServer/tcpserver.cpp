#include "tcpserver.h"
#include <QCryptographicHash>
#include <QThread>

TcpServer::TcpServer(QObject *parent)
    :QTcpServer(parent) {
    m_socket = new QTcpSocket(this);
    connect(this, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    // connect(m_socket, &QTcpSocket::bytesWritten, this, &TcpServer::sendNextPacket); // 写入并发送报文

}

void TcpServer::onNewConnection() {
    m_socket = nextPendingConnection(); // 获取新连接的客户端socket
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead); // 设置读取数据信号和槽
}

// 处理客户端的请求
void TcpServer::onReadyRead() {
    m_socket = qobject_cast<QTcpSocket *>(sender());

    QByteArray data = m_socket->readAll();

    // qDebug() << "收到协议包：" << data.toHex();

    // 解协议
    emit readyForProtocolParse(data);
    qDebug() << "发射readyForProtocolParse";

    // ushort request;
    // in >> request;
    // qDebug() << "收到指令字:" << Qt::hex << Qt::showbase << request;

    // switch (request) {
    //     case GET_FILE_LIST: { // 获取文件列表
    //         emit getFileList();
    //         break;
    //     }
    //     case GET_FILE_TREE: { // 获取文件树
    //         emit getFileTree();
    //         break;
    //     }
    //     case REQUEST_UPLOAD_FILE: { // 客户端请求上传文件
    //         emit requestUploadFile(in);
    //         break;
    //     }
    //     case UPLOAD_FILE: { // 开始发送文件数据
    //         emit uploadFileReceived(in);
    //         break;
    //     }
    //     case REQUEST_DOWNLOAD_FILE: { // 请求下载文件
    //         emit requestDownloadFile(in);
    //         break;
    //     }
    //     case RECEIVE_FILE_READY: { // 客户端准备好接收文件
    //         emit receiveFileReady();
    //         break;
    //     }
    //     case REQUEST_DELETE_FILE: { // 请求删除文件
    //         emit requestDeleteFile(in);
    //         break;
    //     }
    //     default:{
    //         break;
    //     }
    // }
}

// 发送报文
void TcpServer::enqueuePacket(const QByteArray &packet) {
    // qDebug() << "正在执行enqueuePacket";
    // qDebug() << "发送协议包：" << packet.toHex();
    m_socket->write(packet); // 将文件数据写入
    // m_socket->flush(); // 刷新socket
    // m_packetQueue.enqueue(packet);
    // if (!m_isSending) {
    //     m_isSending = true;
        // sendNextPacket();
    // }
}





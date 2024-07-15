 #include "mediator.h"

Mediator::Mediator(
    FileServer *fileServer,
    TcpServer *tcpServer,
    UdpServer *udpServer,
    ProtocolPacketFactory *ppf,
    DataPacketFactory *dpf,
    QObject *parent
): QObject(parent)
    , m_fileServer(fileServer)
    , m_tcpServer(tcpServer)
    , m_udpServer(udpServer)
    , m_ppf(ppf)
    , m_dpf(dpf)
{
    // 解析协议包
    connect(m_tcpServer, &TcpServer::readyForProtocolParse, m_ppf, &ProtocolPacketFactory::parseProtocolPacket);
    connect(m_udpServer, &UdpServer::readyForProtocolParse, m_ppf, &ProtocolPacketFactory::parseProtocolPacket);

    // 解析数据包
    connect(m_ppf, &ProtocolPacketFactory::parsedDataPacket, m_dpf, &DataPacketFactory::parseDataPacket);

    // 封装数据包为协议包
    connect(m_fileServer, &FileServer::readyForWrap, m_ppf, &ProtocolPacketFactory::wrapDataPacket);

    // 发送协议包
    connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_tcpServer, &TcpServer::enqueuePacket);
    connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_udpServer, &UdpServer::sendDatagram);

    // 发送文件树
    connect(m_dpf, &DataPacketFactory::getFileTree, m_fileServer, &FileServer::sendFileTree);

    // 发送文件列表
    connect(m_dpf, &DataPacketFactory::getFileList, m_fileServer, &FileServer::sendFileList);

    // 请求上传文件
    connect(m_dpf, &DataPacketFactory::requestUploadFile, m_fileServer, &FileServer::responseUpload);

    // 接收上传文件
    connect(m_dpf, &DataPacketFactory::uploadFileReceived, m_fileServer, &FileServer::receiveUpload);

    // 请求下载文件
    connect(m_dpf, &DataPacketFactory::requestDownloadFile, m_fileServer, &FileServer::responseDownload);

    // 客户端准备好接收文件
    connect(m_dpf, &DataPacketFactory::receiveFileReady, m_fileServer, &FileServer::sendDownload);

    // 请求删除文件
    connect(m_dpf, &DataPacketFactory::requestDeleteFile, m_fileServer, &FileServer::deleteFile);


}

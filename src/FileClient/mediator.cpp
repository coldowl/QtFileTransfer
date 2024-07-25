 #include "mediator.h"

Mediator::Mediator(
    FileClient *fileClient,
    TcpClient *tcpClient,
    UdpClient *udpClient,
    // SyncFileTransferUI *syncFileFileTransferUI,
    ProtocolPacketFactory *ppf,
    DataPacketFactory *dpf,
    bool selectedProtocol,
    QObject *parent
): QObject(parent)
    , m_fileClient(fileClient)
    , m_tcpClient(tcpClient)
    , m_udpClient(udpClient)
    , m_ppf(ppf)
    , m_dpf(dpf)
    // , m_syncFileFileTransferUI(syncFileFileTransferUI)
    , m_selectedProtocol(selectedProtocol)
{
    // 解析协议包
    connect(m_tcpClient, &TcpClient::readyForProtocolParse, m_ppf, &ProtocolPacketFactory::parseProtocolPacket);
    connect(m_udpClient, &UdpClient::readyForProtocolParse, m_ppf, &ProtocolPacketFactory::parseProtocolPacket);

    // 解析数据包
    connect(m_ppf, &ProtocolPacketFactory::parsedDataPacket, m_dpf, &DataPacketFactory::parseDataPacket);

    // 封装数据包为协议包
    connect(m_fileClient, &FileClient::readyForWrap, m_ppf, &ProtocolPacketFactory::wrapDataPacket);

    // 发送协议包
    connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_tcpClient, &TcpClient::enqueuePacket);
    connect(m_ppf, &ProtocolPacketFactory::wrappedProtocolPacket, m_udpClient, &UdpClient::sendDatagram);


    // 若TCP连接成功，先请求文件树
    connect(m_tcpClient, &TcpClient::tcpConnectSuccess, m_fileClient, &FileClient::requestFileTree);

    // 若UDP设置成功，先请求文件树
    connect(m_udpClient, &UdpClient::setUdpConnectInfoSuccess, m_fileClient, &FileClient::requestFileTree);


    // 解析文件树
    connect(m_dpf, &DataPacketFactory::fileTreeReceived, m_fileClient, &FileClient::dealFileTree);

    // 处理文件列表
    connect(m_dpf, &DataPacketFactory::fileListReceived, m_fileClient, &FileClient::dealFileList);

    // 上传文件
    connect(m_dpf, &DataPacketFactory::fileUploadReady, m_fileClient, &FileClient::uploadFile);

    // 下载之前的准备
    connect(m_dpf, &DataPacketFactory::fileDownloadReady, m_fileClient, &FileClient::prepareForFileDownload);

    // 接收下载文件
    connect(m_dpf, &DataPacketFactory::downloadFileReceived, m_fileClient, &FileClient::receiveDownload);
}

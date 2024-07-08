#include "mediator.h"

Mediator::Mediator(
    FileClient *fileClient,
    TcpClient *tcpClient,
    FileTransferWidget *fileTransferWidget,
    // ProtocolPacketFactory *ppf,
    // DataPacketFactory *dpf,
    QObject *parent
): QObject(parent)
    , m_fileClient(fileClient)
    , m_tcpClient(tcpClient)
    , m_fileTransferWidget(fileTransferWidget)
/*    , m_ppf(ppf)
    , m_dpf(dpf)*/{

    // 数据包准备好封装
    connect(m_fileClient, &FileClient::readyForWrap, m_tcpClient, &TcpClient::enqueuePacket);



    connect(m_fileClient, &FileClient::uploadBasicInfo, m_fileTransferWidget, &FileTransferWidget::setBasicInfo);
    connect(m_fileClient, &FileClient::uploadProgressInfo, m_fileTransferWidget, &FileTransferWidget::setProgressInfo);


    // 若连接成功，先请求文件树
    connect(m_tcpClient, &TcpClient::tcpConnectSuccess, m_fileClient, &FileClient::requestFileTree);

    // 解析文件树
    connect(m_tcpClient, &TcpClient::fileTreeReceived, m_fileClient, &FileClient::dealFileTree);

    // 处理文件列表
    connect(m_tcpClient, &TcpClient::fileListReceived, m_fileClient, &FileClient::dealFileList);

    // 上传文件
    connect(m_tcpClient, &TcpClient::fileUploadReady, m_fileClient, &FileClient::uploadFile);

    // 下载之前的准备
    connect(m_tcpClient, &TcpClient::fileDownloadReady, m_fileClient, &FileClient::prepareForFileDownload);

    // 接收下载文件
    connect(m_tcpClient, &TcpClient::downloadFileReceived, m_fileClient, &FileClient::receiveDownload);
}

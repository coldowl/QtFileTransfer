#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include "fileclient.h"
#include "tcpclient.h"
#include "udpclient.h"
#include "protocolpacketfactory.h"
#include "datapacketfactory.h"
// #include "syncfiletransferui.h"

class Mediator : public QObject
{
    Q_OBJECT
public:
    explicit Mediator(
        FileClient *m_fileClient,
        TcpClient *m_tcpClient,
        UdpClient *m_udpClient,
        // SyncFileTransferUI *m_syncFileFileTransferUI,
        ProtocolPacketFactory *m_ppf,
        DataPacketFactory *m_dpf,
        bool m_selectedProtocol,
        QObject *parent = nullptr
    );

private:
    FileClient *m_fileClient = nullptr;
    TcpClient *m_tcpClient = nullptr;
    UdpClient *m_udpClient = nullptr;
    ProtocolPacketFactory *m_ppf = nullptr;
    DataPacketFactory *m_dpf = nullptr;
    // SyncFileTransferUI *m_syncFileFileTransferUI = nullptr;
    bool m_selectedProtocol = 0; // 默认使用TCP
};

#endif // MEDIATOR_H

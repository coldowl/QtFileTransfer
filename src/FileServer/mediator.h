#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include "fileserver.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "protocolpacketfactory.h"
#include "datapacketfactory.h"


class Mediator : public QObject
{
    Q_OBJECT
public:
    explicit Mediator(
        FileServer *m_fileClient,
        TcpServer *m_tcpClient,
        UdpServer *m_udpServer,
        ProtocolPacketFactory *m_ppf,
        DataPacketFactory *m_dpf,
        QObject *parent = nullptr
    );

private:
    FileServer *m_fileServer = nullptr;
    TcpServer *m_tcpServer = nullptr;
    UdpServer *m_udpServer = nullptr;
    ProtocolPacketFactory *m_ppf = nullptr;
    DataPacketFactory *m_dpf = nullptr;
};

#endif // MEDIATOR_H

#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include "fileclient.h"
#include "tcpclient.h"
#include "protocolpacketfactory.h"
#include "datapacketfactory.h"
#include "filetransferwidget.h"

class Mediator : public QObject
{
    Q_OBJECT
public:
    explicit Mediator(
        FileClient *m_fileClient,
        TcpClient *m_tcpClient,
        FileTransferWidget *m_fileTransferWidget,
        // ProtocolPacketFactory *m_ppf,
        // DataPacketFactory *m_dpf,
        QObject *parent = nullptr
    );

private:
    FileClient *m_fileClient = nullptr;
    TcpClient *m_tcpClient = nullptr;
    ProtocolPacketFactory *m_ppf = nullptr;
    DataPacketFactory *m_dpf = nullptr;
    FileTransferWidget *m_fileTransferWidget = nullptr;
};

#endif // MEDIATOR_H

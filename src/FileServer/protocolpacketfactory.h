#ifndef PROTOCOLPACKETFACTORY_H
#define PROTOCOLPACKETFACTORY_H

#include <QObject>

class ProtocolPacketFactory : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolPacketFactory(QObject *parent = nullptr);

signals:
    // 协议包(已封装好)
    void wrappedProtocolPacket(const QByteArray &protocolPacket);

    // 数据包(已解析好)
    void parsedDataPacket(QByteArray &dataPacket);

public slots:
    // 解析协议包
    void parseProtocolPacket(const QByteArray &data);

    // 封装数据包
    void wrapDataPacket(const QByteArray &dataPacket);

private:
    // 存储协议包的缓存
    QByteArray buffer;
};

#endif // PROTOCOLPACKETFACTORY_H

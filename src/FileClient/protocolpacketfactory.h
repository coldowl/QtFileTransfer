#ifndef PROTOCOLPACKETFACTORY_H
#define PROTOCOLPACKETFACTORY_H

#include <QObject>

class ProtocolPacketFactory : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolPacketFactory(QObject *parent = nullptr);

signals:
    // 协议包(已封装好）
    void wrappedProtocolPacket(const QByteArray &protocolPacket);

    // 数据包（已解析好）
    void parsedDataPacket(const QByteArray &dataPacket);

private slots:
    // 解析协议包
    void parseProtocolPacket(QDataStream &in);

    // 封装数据包
    void wrapDataPacket(const QByteArray &dataPacket);

private:
    // 判断是否误码
    bool checkSum(const QByteArray &packet, int len);
};

#endif // PROTOCOLPACKETFACTORY_H

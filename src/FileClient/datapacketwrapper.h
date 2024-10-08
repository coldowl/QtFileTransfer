#ifndef DATAPACKETWRAPPER_H
#define DATAPACKETWRAPPER_H

#include <QObject>
#include <tcpclient.h>
#include <fileclient.h>

class DataPacketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DataPacketWrapper(QObject *parent = nullptr);

signals:
    // 协议包(已封装好）
    void wrappedProtocolPacket(const QByteArray &protocolPacket);

    // 数据包（已解析好）
    void parsedDataPacket(const QByteArray &dataPacket);

private slots:
    // 解析协议包
    void parseProtocolPacket(QByteArray &buffer, const QByteArray &newProtocolPacket);

    // 封装数据包
    void wrapDataPacket(const QByteArray &dataPacket);

private:
    // 判断是否误码
    bool checkSum(const QByteArray &packet, int len);

    FileClient *m_fileClient = nullptr;

};

#endif // DATAPACKETWRAPPER_H

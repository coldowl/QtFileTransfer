#ifndef DATAPACKETWRAPPER_H
#define DATAPACKETWRAPPER_H

#include <QObject>
#include <tcpclient.h>

class DataPacketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DataPacketWrapper(QObject *parent = nullptr);

signals:
    // 完整的协议包
    void totalProtocolPacket(const QByteArray &packet);

    // 完整的数据包
    void totalDataPacket(const QByteArray &packet);

private slots:
    // 解析协议包
    void parsePacket(QByteArray &buffer, const QByteArray &newData);

    // 封装数据包
    void wrapPacket(const QByteArray &packet);

private:
    // 判断是否误码
    bool checkSum(const QByteArray &packet, int len);

};

#endif // DATAPACKETWRAPPER_H

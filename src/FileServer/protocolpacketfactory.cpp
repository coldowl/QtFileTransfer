#include "protocolpacketfactory.h"

#include <QByteArray>
#include <QDebug>
#include <QDataStream>

ProtocolPacketFactory::ProtocolPacketFactory(QObject *parent)
    :QObject(parent){
}

// 组协议包
void ProtocolPacketFactory::wrapDataPacket(const QByteArray &dataPacket) {

    QByteArray result;
    quint16 HEAD = 0xFEEF;

    // 计算总长度LEN（HEAD长度2字节 + LEN长度2字节 + 数据包长度 + SUMCHECK长度1字节）
    quint16 LEN = 2 + 2 + dataPacket.size() + 1;

    // 添加 HEAD + LEN + packet 数据
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << HEAD << LEN;

    // 逐字节写入 packet ,防止自动添加长度
    for (char byte : dataPacket) {
        stream << static_cast<quint8>(byte);
    }

    // 计算SUMCHECK（累加和）
    quint8 SUMCHECK = 0;
    for (char byte : result) {
        SUMCHECK += byte;
    }

    // 将累加和添加到结果的末尾
    stream << SUMCHECK;

    // qDebug() << "HEAD:" << Qt::hex << HEAD;
    // qDebug() << "LEN:" << LEN;
    // qDebug() << "packet:" << packet.toHex();
    // qDebug() << "SUMCHECK:" << Qt::hex << SUMCHECK;
    // qDebug() << "result:" << result.toHex();

    emit wrappedProtocolPacket(result);
}

// 解协议包，解出（ID+DATA）
void ProtocolPacketFactory::parseProtocolPacket(const QByteArray &data) {

    // Step (2) 将数据包追加到缓存
    buffer.append(data);

    while (true) {
        // Step (3) 查找包头HEAD
        int headIndex = buffer.indexOf(QByteArray::fromHex("FEEF"));
        if (headIndex == -1) {
            // 若不存在HEAD，清空缓存，返回
            buffer.clear();
            return;
        }

        // Step (4) 判断从HEAD开始到缓存底部的长度是否大于等于7个字节
        // 即判断缓存中是否可能存在一个完整的包
        if (buffer.size() - headIndex < 7) {
            // 若不足7个字节，等待更多数据
            return;
        }

        // Step (5) 提取长度字段LEN
        int len = static_cast<unsigned char>(buffer.at(headIndex + 2)) << 8 | static_cast<unsigned char>(buffer.at(headIndex + 3));

        // 判断buffer从headIndex到末尾的长度是否小于len
        // 即判断当前所在包是否完整
        if (buffer.size() - headIndex < len) {
            // 若不足，等待更多数据
            return;
        }

        // Step (6) 提取校验字段SUMCHECK
        QByteArray packet = buffer.mid(headIndex, len);
        unsigned char sumCheck = static_cast<unsigned char>(packet.at(packet.size()- 1));
        unsigned char calculatedSum = 0;
        for (int i = 0; i < packet.size() - 1; ++i) {
            calculatedSum += static_cast<unsigned char>(packet.at(i));
        }

        if (calculatedSum != sumCheck) {
            // 校验和不匹配，清除当前包，继续查找下一个HEAD
            buffer.remove(headIndex, len);
            continue;
        }

        // 若校验和匹配，处理当前包
        QByteArray data = packet.mid(5, len - 7); // 提取DATA部分
        // qDebug() << "Received packet with ID:" << packet.mid(4, 2).toHex() << "Data:" << data.toHex();

        // 送数据包去解析
        QByteArray dataPacket = packet.mid(4, len-5);
        // qDebug() << dataPacket.toHex();

        emit parsedDataPacket(dataPacket);


        // Step (7) 从缓存中清除当前包
        buffer.remove(headIndex, len);
    }
}

#include "datapacketwrapper.h"

#include <QByteArray>
#include <QDebug>


DataPacketWrapper::DataPacketWrapper(QObject *parent)
    :QObject(parent){

}

// 组协议包，从（ID+DATA）开始
void DataPacketWrapper::wrapPacket(const QByteArray &packet) {
    // 创建一个QByteArray对象用于存储结果
    QByteArray result;

    // 头部HEAD
    quint16 HEAD = 0xFEEF;

    // 计算总长度LEN（HEAD长度2字节 + LEN长度2字节 + packet长度 + SUM长度1字节）
    quint16 LEN = 2 + 2 + packet.size() + 1;

    // 添加 HEAD + LEN + packet 数据
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << HEAD << LEN;

    // 逐字节写入 packet ,防止自动添加长度
    for (char byte : packet) {
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

    emit totalProtocolPacket(result);
}

// 解协议包，解出（ID+DATA）
void DataPacketWrapper::parsePacket(QByteArray& buffer, const QByteArray& newData) {
    // 将新的数据追加到缓存
    buffer.append(newData);

    while (true) {
        // 查找包头HEAD（0xHEAD为包头标识，可以根据实际情况修改）
        int headIndex = buffer.indexOf(QByteArray::fromHex("HEAD"));
        if (headIndex == -1) {
            // 包头不存在，清空缓存并返回
            buffer.clear();
            return;
        }

        // 判断从HEAD开始到缓存底部的长度是否大于等于7个字节
        if (buffer.size() - headIndex < 7) {
            // 长度不足，清空缓存并返回
            buffer.clear();
            return;
        }

        // 提取长度字段LEN（假设LEN为无符号16位整数）
        quint16 len = (static_cast<quint8>(buffer[headIndex + 2]) << 8) | static_cast<quint8>(buffer[headIndex + 3]);

        // 判断缓存剩余的数据是否大于等于（LEN-2）
        if (buffer.size() - headIndex < len + 5) {
            // 数据不足，返回等待更多数据
            return;
        }

        // 提取完整包（从HEAD开始，长度为LEN + 5）
        QByteArray packet = buffer.mid(headIndex, len + 5);

        // 校验SUMCHECK
        if (!checkSum(packet, len + 4)) {
            // 校验失败，移除当前包头并继续查找下一个包头
            buffer.remove(0, headIndex + 2);
            continue;
        }

        // 处理有效的数据包
        QByteArray data = packet.mid(6, len - 2);
        emit totalDataPacket(data);

        // 从缓存中清除当前包
        buffer.remove(0, headIndex + len + 5);
    }
}


// 检查数据包的校验和
bool DataPacketWrapper::checkSum(const QByteArray &packet, int len) {
    quint8 sum = 0;
    for (int i = 0; i < len; ++i) {
        sum += static_cast<quint8>(packet[i]);
    }
    return sum == static_cast<quint8>(packet[len]);
}


#include "datapacketfactory.h"
#include <QDebug>
#include <QDataStream>

DataPacketFactory::DataPacketFactory(QObject *parent)
    : QObject(parent){

}

void DataPacketFactory::parseDataPacket(QByteArray &dataPacket){
    QDataStream in(&dataPacket, QIODevice::ReadOnly);
    ushort command;
    in >> command; // 读取指令字
    qDebug() << "收到指令字:" << Qt::hex << Qt::showbase << command;

    QByteArray remainingData = dataPacket.mid(sizeof(ushort)); // 跳过command

    switch(command) {
    case FILE_TREE: { // 文件树
        emit fileTreeReceived(remainingData);
        break;
    }
    case FILE_LIST: { // 文件列表
        emit fileListReceived(remainingData);
        break;
    }
    case UPLOAD_FILE_READY: { // 准备上传文件
        emit fileUploadReady();
        break;
    }
    case UPLOAD_COMPLETE: { // 上传完成
        // 上传完成后的处理
        break;
    }
    case DOWNLOAD_FILE_READY: { // 准备下载文件
        emit fileDownloadReady(remainingData);
        break;
    }
    case DOWNLOAD_FILE: { // 下载文件
        emit downloadFileReceived(remainingData);
        break;
    }
    default:{ // 其他指令类型处理
        qWarning() << "未知指令字:" << command;
        break;
    }
    }
}

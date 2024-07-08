#include "datapacketfactory.h"
#include <QDebug>

DataPacketFactory::DataPacketFactory(QObject *parent)
    : QObject(parent){

}

void DataPacketFactory::parseDataPacket(QDataStream &in){

    ushort command;
    // in >> command; // 读取指令字
    qDebug() << "收到指令字:" << Qt::hex << Qt::showbase << command;

    switch(command) {
    case FILE_TREE: { // 文件树
        emit fileTreeReceived(in);
        break;
    }
    case FILE_LIST: { // 文件列表
        emit fileListReceived(in);
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
        emit fileDownloadReady(in);
        break;
    }
    case DOWNLOAD_FILE: { // 下载文件
        emit downloadFileReceived(in);
        break;
    }
    default:{ // 其他指令类型处理
        qWarning() << "未知指令字:" << command;
        break;
    }
    }
}

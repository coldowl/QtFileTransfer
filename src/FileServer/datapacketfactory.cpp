#include "datapacketfactory.h"
#include <QDebug>
#include <QDataStream>

DataPacketFactory::DataPacketFactory(QObject *parent)
    : QObject(parent){

}

void DataPacketFactory::parseDataPacket(QByteArray &dataPacket){
    QDataStream in(&dataPacket, QIODevice::ReadOnly);
    qDebug() << "正在执行parseDataPacket";
    ushort command;
    in >> command; // 读取指令字
    qDebug() << "收到指令字:" << Qt::hex << Qt::showbase << command;

    QByteArray remainingData = dataPacket.mid(sizeof(ushort)); // 跳过command


    switch (command) {
        case GET_FILE_LIST: { // 获取文件列表
            emit getFileList();
            break;
        }
        case GET_FILE_TREE: { // 获取文件树
            emit getFileTree();
            break;
        }
        case REQUEST_UPLOAD_FILE: { // 客户端请求上传文件
            emit requestUploadFile(remainingData);
            break;
        }
        case UPLOAD_FILE: { // 开始发送文件数据
            emit uploadFileReceived(remainingData);
            break;
        }
        case REQUEST_DOWNLOAD_FILE: { // 请求下载文件
            emit requestDownloadFile(remainingData);
            break;
        }
        case RECEIVE_FILE_READY: { // 客户端准备好接收文件
            emit receiveFileReady();
            break;
        }
        case REQUEST_DELETE_FILE: { // 请求删除文件
            emit requestDeleteFile(remainingData);
            break;
        }
        case DOWNLOAD_COMPLETE: { // 客户端确认下载过程无误
            emit downloadComplete();
            break;
        }
        default:{
            qWarning() << "未知指令字:" << Qt::hex << Qt::showbase << command;
            break;
        }
    }
}

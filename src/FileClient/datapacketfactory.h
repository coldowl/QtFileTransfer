#ifndef DATAPACKETFACTORY_H
#define DATAPACKETFACTORY_H

#include <QObject>


//服务器发出的指令0x1开头
#define FILE_TREE 0x1001
#define FILE_LIST 0x1002
#define UPLOAD_FILE_READY 0x1003
#define UPLOAD_COMPLETE 0x1004
#define DOWNLOAD_FILE_READY 0x1005
#define DOWNLOAD_FILE 0x1006
#define RECEIVE_FILE_READY 0x1007

//客户端发出的指令0x2开头
#define GET_FILE_TREE 0x2001
#define GET_FILE_LIST 0x2002
#define REQUEST_UPLOAD_FILE 0x2003
#define REQUEST_DOWNLOAD_FILE 0x2004
#define REQUEST_DELETE_FILE 0x2005
#define UPLOAD_FILE 0x2006
#define DOWNLOAD_COMPLETE 0x2007

class DataPacketFactory : public QObject
{
    Q_OBJECT
public:
    explicit DataPacketFactory(QObject *parent = nullptr);

signals:
    // 文件树
    void fileTreeReceived(QDataStream &in);

    // 文件列表
    void fileListReceived(QDataStream &in);

    // 准备上传文件
    void fileUploadReady();

    // 准备下载文件
    void fileDownloadReady(QDataStream &in);

    // 下载文件
    void downloadFileReceived(QDataStream &in);

    // TCP建立连接成功
    void tcpConnectSuccess();

public slots:
    void parseDataPacket(QDataStream &in);
};

#endif // DATAPACKETFACTORY_H

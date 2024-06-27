#ifndef FILECLIENT_H
#define FILECLIENT_H

#include "tcpclient.h"
#include <QString>
#include <QStandardItem>
#include <QTcpSocket>
#include <QTreeView>
#include <QFile>

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



class FileClient : public QObject{
    Q_OBJECT

public:
    // 构造函数
    explicit FileClient(TcpClient *tcpClient,QTreeView *view =nullptr, QObject *parent = nullptr);

    // 请求文件列表
    void requestFileList();

    // 请求文件树
    void requestFileTree();

    // 请求上传文件
    void requestUpload(const QString &filePath);

    // 请求下载文件
    void requestDownload(const QString &fileName);

    // 请求删除文件
    void requestDelete(const QString &fileName);

private slots:
    // 处理文件树
    void dealFileTree(QDataStream &in);

    // 处理文件列表
    void dealFileList(QDataStream &in);

private:
    // 上传文件
    void uploadFile();

    // 下载之前的准备工作
    void prepareForFileDownload(QDataStream &in);

    // 接收下载文件
    void receiveDownload(QDataStream &in);

    // 递归解析目录结构
    void parseDirectory(QDataStream &in, QStandardItem *parentItem);

private:

    TcpClient *m_tcpClient = nullptr;
    QTreeView *m_treeView = nullptr;
    QStandardItemModel *model = nullptr;

    QString m_uploadFilePath = "";
    QFile *m_uploadFile = nullptr;


    QString m_downloadFileName = "";
    qint64 m_downloadFileSize = 0;
    qint64 m_downloadBytesReceived = 0;
    QString m_downloadFileHash = "";
    QString m_downloadFilePath = "";

};

#endif // FILECLIENT_H

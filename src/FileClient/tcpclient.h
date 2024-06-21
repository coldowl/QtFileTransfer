#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDataStream>
#include <QDir>


class TcpClient : public QObject {
    Q_OBJECT

public:
    // 构造函数，初始化treeView和model
    explicit TcpClient(QTreeView *view, QObject *parent = nullptr);

    // 连接到服务器
    bool connectToServer(const QString &host, const quint16 port);

    // 断开连接
    bool disconnectFromServer();

    // 请求文件树
    void requestFileTree();

    // 请求上传文件
    void requestUpload(const QString &filePath);

    // 请求下载文件
    void requestDownload(const QString &fileName);

    // 请求删除文件
    void requestDelete(const QString &fileName);

private slots:
    // 读取服务器发送的数据
    void onReadyRead();

private:
    // 上传文件
    void uploadFile();

    // 接收下载文件
    void receiveDownload(QDataStream &in);

    // 请求文件列表
    void requestFileList();

    // 请求下载文件
    void requestDownload();

    // 请求删除文件
    void requestDelete();

    // 递归解析目录结构
    void parseDirectory(QDataStream &in, QStandardItem *parentItem);

    QTcpSocket *m_socket = nullptr;
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

#endif //TCPCLIENT_H

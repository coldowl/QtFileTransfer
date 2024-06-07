#include "tcpclient.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QIcon>


TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , socket(new QTcpSocket(this))
    , model(new QFileSystemModel(this)){

    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::handleServerResponse);
}

//  连接到服务器
bool TcpClient::connectToServer(const QString &address, quint16 port) {

    socket->connectToHost(address, port);  //  尝试连接到服务器

    // 等待连接结果，超时为5秒
    if (socket->waitForConnected(5000)) {
        return true;
    } else {
        qWarning() << "连接到服务器失败:" << socket->errorString();
        return false;
    }
}

//  请求文件列表
void TcpClient::requestDirectoryList(const QString &path) {
    QByteArray request = "LIST " + path.toUtf8();
    socket->write(request);
}


//  请求文件详细信息
void TcpClient::requestFileDetails(const QString &path) {
    QByteArray request = "DETAILS " + path.toUtf8();
    socket->write(request);
}

//  请求删除文件
void TcpClient::requestDelete(const QString &path) {
    QByteArray request = "DELETE " + path.toUtf8();
    socket->write(request);
}

//  处理 Server 回复报文
void TcpClient::handleServerResponse() {
    while (socket->bytesAvailable() > 0) {
        QByteArray response = socket->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

        if (jsonDoc.isArray()) {
            QJsonArray jsonArray = jsonDoc.array();
            updateListView(jsonArray);
        } else if (jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();
            if (jsonObject.contains("path")) {
                updateDetailsView(jsonObject);
            } else {
                // 处理删除操作的响应等其他对象响应
                QString result = jsonObject["result"].toString();
                if (result == "Delete successful") {
                    // 处理删除成功
                } else {
                    // 处理删除失败或其他响应
                }
            }
        }
    }
}

void TcpClient::updateListView(const QJsonArray &jsonEntries) {
    // 清空模型中的现有数据
    model->removeRows(0, model->rowCount());

    for (const QJsonValue &entryValue : jsonEntries) {
        QJsonObject entryObject = entryValue.toObject();
        QString name = entryObject["name"].toString();
        bool isDir = entryObject["isDir"].toBool();

        // 这里我们手动添加项目到模型中，示例代码假设模型直接与视图绑定
        QStandardItem *item = new QStandardItem(name);
        if (isDir) {
            item->setIcon(QIcon::fromTheme("folder"));
        } else {
            item->setIcon(QIcon::fromTheme("text-plain"));
        }
        // model->appendRow(item);
    }

    // 更新视图
    ui->listView->setModel(model);
}

void TcpClient::updateDetailsView(const QJsonObject &jsonObject) {
    QString details;
    details = QString("Path: %1\nSize: %2\nType: %3\nLast Modified: %4")
                  .arg(jsonObject["path"].toString())
                  .arg(jsonObject["size"].toInt())
                  .arg(jsonObject["type"].toString())
                  .arg(jsonObject["lastModified"].toString());

    ui->datailTextEdit->setText(details);
}

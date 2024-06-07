#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QFileSystemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ui_mainwindow.h"

class TcpClient : public QObject {
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);

    bool connectToServer(const QString &address, quint16 port);
    void requestDirectoryList(const QString &path);
    void requestFileDetails(const QString &path);
    void requestDelete(const QString &path);

private slots:
    void handleServerResponse();

private:
    QTcpSocket *socket;
    QFileSystemModel *model;
    Ui::MainWindow *ui;

    void updateListView(const QJsonArray &jsonEntries);
    void updateDetailsView(const QJsonObject &jsonObject);
};


#endif // TCPCLIENT_H

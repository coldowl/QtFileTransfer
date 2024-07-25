#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileSystemModel>
#include <QSpinBox>
#include <QLabel>
#include "tcpserver.h"
#include "udpserver.h"
#include "fileserver.h"
#include "protocolpacketfactory.h"
#include "datapacketfactory.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onListViewClicked(const QModelIndex &index);

    void on_actListen_triggered();

    void on_actDisconnect_triggered();

    void on_actChooseFolder_triggered();

    void onSocketStateChanged(QString stateString);
    void processNextState();
private:
    Ui::MainWindow *ui;
    QSpinBox *m_spinPortEdit;
    QLabel *m_labSocketState;
    QFileSystemModel *m_model;//定义数据模型变量

    QTcpSocket *m_tcpsocket; // 定义TCP socket
    TcpServer* m_tcpServer;
    UdpServer *m_udpServer;
    FileServer *m_fileServer;
    ProtocolPacketFactory *m_ppf;
    DataPacketFactory *m_dpf;

    QStringList m_stateQueue;
    bool m_processing = false;

private:
    QString getLocalIP();


};
#endif // MAINWINDOW_H

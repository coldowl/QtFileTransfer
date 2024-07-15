#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include "tcpclient.h"
#include "udpclient.h"
#include "fileclient.h"
#include "filetransferwidget.h"
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

    void on_actConnect_triggered();

    void on_actDisconnect_triggered();

    void on_actUpload_triggered();

    void on_refreshButton_clicked();

    void on_actDelete_triggered();

    void on_actDownload_triggered();

    void onProtocolChanged(int index);
private:
    Ui::MainWindow *ui;
    FileTransferWidget *fileTransferWidget;

    QFileSystemModel *model;//定义数据模型变量
    QString getLocalIP();

    TcpClient *m_tcpClient;
    UdpClient *m_udpClient;
    FileClient *m_fileClient;
    FileTransferWidget *m_fileTransferWidget;
    ProtocolPacketFactory *m_ppf;
    DataPacketFactory *m_dpf;

    bool m_selectedProtocol = 0; // 0表示选择TCP协议，1表示选择UDP协议，默认是0

    QProgressBar *downloadProgressBar;
    QLineEdit *lineServerIp;
    QSpinBox *spinPortEdit;
    QComboBox *protocolComboBox;

};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include "tcpclient.h"
#include "udpclient.h"
#include "fileclient.h"
#include "filetransferwidget.h"
#include "protocolpacketfactory.h"
#include "datapacketfactory.h"
#include "syncfiletransferui.h"


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

    void displayState(const QString &message);

    void onSocketStateChanged(QString stateString);

    void processNextState();

    void initUi();
    void threadHandle();
    void connectSet();
signals:
    void startTcpConnect(const QString ip, quint16 port);
    void startUdpConnect(const QString ip, quint16 port);
    void getModel(QStandardItemModel* &model);
    void tcpDisconnect();
    void udpDisconnect();
    void requestUpload(QString path);
    void requestFileTree();
    void requestDelete(QString fileName);
    void requestDownload(QString fileName);

private:
    Ui::MainWindow *ui;
    FileTransferWidget *fileTransferWidget;

    QFileSystemModel *m_model;//定义数据模型变量
    QString getLocalIP();

    TcpClient *m_tcpClient;
    UdpClient *m_udpClient;
    FileClient *m_fileClient;
    FileTransferWidget *m_fileTransferWidget;
    SyncFileTransferUI *m_syncFileFileTransferUI;
    ProtocolPacketFactory *m_ppf;
    DataPacketFactory *m_dpf;
    QThread *m_tcpThread;
    QThread *m_udpThread;
    QThread *m_fileThread;

    bool m_selectedProtocol = 0; // 0表示选择TCP协议，1表示选择UDP协议，默认是0

    // 桌面控件
    QLineEdit *m_lineServerIp;
    QSpinBox *m_spinPortEdit;
    QComboBox *m_protocolComboBox;
    QLabel *m_labSocketState;

    QStringList m_stateQueue;
    bool m_processing = false;


};
#endif // MAINWINDOW_H

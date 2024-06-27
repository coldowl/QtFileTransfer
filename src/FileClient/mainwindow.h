#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QLineEdit>
#include <QSpinBox>
#include "tcpclient.h"
#include "fileclient.h"


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

    void on_pushButton_clicked();

    void on_actDelete_triggered();

    void on_actDownload_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;//定义数据模型变量
    QString getLocalIP();
    TcpClient *m_tcpClient;
    FileClient *m_fileClient;
    QProgressBar *downloadProgressBar;
    QLineEdit *lineServerIp;
    QSpinBox *spinPortEdit;



};
#endif // MAINWINDOW_H

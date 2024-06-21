#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileSystemModel>
#include <QSpinBox>
#include "tcpserver.h"


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
    void on_actUpload_triggered();

    void on_actListen_triggered();

    void on_actDisconnect_triggered();


    void on_actChooseFolder_triggered();

private:
    Ui::MainWindow *ui;
    QSpinBox *spinPortEdit;
    QFileSystemModel *model;//定义数据模型变量
    QTcpSocket *m_tcpsocket; // 定义TCP socket
    TcpServer* m_tcpserver;//

private:
    QString getLocalIP();


};
#endif // MAINWINDOW_H

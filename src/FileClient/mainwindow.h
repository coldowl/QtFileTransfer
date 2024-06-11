#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QLineEdit>
#include <QSpinBox>
#include "myfileinfo.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


//消息类型
enum MsgType{
    FileInfo,   //文件信息，如文件名，文件大小等信息
    FileData,   //文件数据，即文件内容
};


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void downLoadFile();//
    void readServerMsg();//
    void fileInfoRead();//
    void fileDataRead();//

private slots:
    void onListViewClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;//定义数据模型变量
    QString getLocalIP();
    QTcpSocket* m_tcpSocket;//
    MyFileInfo* myFile;//
    QString m_downloadPath;//
    bool isDownloading; //
    QProgressBar *downloadProgressBar;
    QLineEdit *lineServerIp;
    QSpinBox *spinPortEdit;


private:
    // void connectSigSlots();//
    void connectToServer();//
    bool connectToServer(QTcpSocket *socket);//

};
#endif // MAINWINDOW_H

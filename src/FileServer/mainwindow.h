#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileSystemModel>
#include <QSpinBox>


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
    bool startServer();//

public slots://
    void delNewConnect();//
    void dealMsg(QTcpSocket* socket);//

private slots:
    void onListViewClicked(const QModelIndex &index);
    void on_actUpload_triggered();

    void on_actListen_triggered();

    void on_actDisconnect_triggered();

private:
    Ui::MainWindow *ui;
    QSpinBox *spinPortEdit;
    QFileSystemModel *model;//定义数据模型变量
    QTcpSocket *socket; // 定义TCP socket
    QTcpServer* m_tcpServer;//
    qint64 typeMsgSize;//
    qint64 m_totalBytes;//
    qint64 m_sendFileSize;//
    qint64 m_fileInfoWriteBytes;//
    QString m_sendFilePath;//
    QFile m_localFile;//
private:
    QString getLocalIP();
    void transferFileData(QTcpSocket* socket);//
    void transferFileInfo(QTcpSocket* socket);//
    QByteArray getFileContent(QString filePath);//
    bool checkFile(QString filePath);//
};
#endif // MAINWINDOW_H

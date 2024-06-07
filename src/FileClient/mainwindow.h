#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFileSystemModel>

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

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;//定义数据模型变量
    QTcpSocket *socket; // 定义TCP socket
    QString getLocalIP();

};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include <QApplication>
#include <savelog.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 启动日志记录
    SaveLog::Instance()->start();

    MainWindow w;
    w.show();

    return a.exec();

    // 停止日志记录
    SaveLog::Instance()->stop();
}


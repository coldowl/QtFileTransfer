#include "mainwindow.h"
// #include "tcpclient.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // // Load the QSS file
    // QFile file(":/darkblue.qss");  // Use ":/darkblue.qss" if the QSS file is included as a resource
    // if (file.open(QFile::ReadOnly | QFile::Text))
    // {
    //     QTextStream stream(&file);
    //     QString qss = stream.readAll();
    //     a.setStyleSheet(qss);
    //     file.close();
    // }

    MainWindow w;
    w.show();

    // TcpClient tcpClient;
    // tcpClient.connectToServer("127.0.0.1", 12345);
    return a.exec();
}


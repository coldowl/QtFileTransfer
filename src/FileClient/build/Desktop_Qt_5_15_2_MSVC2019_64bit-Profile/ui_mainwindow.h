/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actConnect;
    QAction *actDisconnect;
    QAction *actUpload;
    QAction *actDownload;
    QAction *actDelete;
    QAction *actionabout;
    QWidget *centralwidget;
    QGridLayout *gridLayout_5;
    QSplitter *splitter_4;
    QSplitter *splitter_3;
    QSplitter *splitter;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QTreeView *treeView;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout;
    QLineEdit *pathLineEdit;
    QListView *listView;
    QSplitter *splitter_2;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QTextEdit *datailTextEdit;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_3;
    QTextEdit *textEdit_2;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menu;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(890, 625);
        actConnect = new QAction(MainWindow);
        actConnect->setObjectName(QString::fromUtf8("actConnect"));
        actDisconnect = new QAction(MainWindow);
        actDisconnect->setObjectName(QString::fromUtf8("actDisconnect"));
        actUpload = new QAction(MainWindow);
        actUpload->setObjectName(QString::fromUtf8("actUpload"));
        actDownload = new QAction(MainWindow);
        actDownload->setObjectName(QString::fromUtf8("actDownload"));
        actDelete = new QAction(MainWindow);
        actDelete->setObjectName(QString::fromUtf8("actDelete"));
        actionabout = new QAction(MainWindow);
        actionabout->setObjectName(QString::fromUtf8("actionabout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_5 = new QGridLayout(centralwidget);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        splitter_4 = new QSplitter(centralwidget);
        splitter_4->setObjectName(QString::fromUtf8("splitter_4"));
        splitter_4->setOrientation(Qt::Vertical);
        splitter_3 = new QSplitter(splitter_4);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        splitter = new QSplitter(splitter_3);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        groupBox_2 = new QGroupBox(splitter);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        treeView = new QTreeView(groupBox_2);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        gridLayout_4->addWidget(treeView, 0, 0, 1, 1);

        splitter->addWidget(groupBox_2);
        groupBox_3 = new QGroupBox(splitter);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout = new QGridLayout(groupBox_3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pathLineEdit = new QLineEdit(groupBox_3);
        pathLineEdit->setObjectName(QString::fromUtf8("pathLineEdit"));

        gridLayout->addWidget(pathLineEdit, 0, 0, 1, 1);

        listView = new QListView(groupBox_3);
        listView->setObjectName(QString::fromUtf8("listView"));

        gridLayout->addWidget(listView, 1, 0, 1, 1);

        splitter->addWidget(groupBox_3);
        splitter_3->addWidget(splitter);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        groupBox_4 = new QGroupBox(splitter_2);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        datailTextEdit = new QTextEdit(groupBox_4);
        datailTextEdit->setObjectName(QString::fromUtf8("datailTextEdit"));

        gridLayout_2->addWidget(datailTextEdit, 0, 0, 1, 1);

        splitter_2->addWidget(groupBox_4);
        groupBox_5 = new QGroupBox(splitter_2);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_3 = new QGridLayout(groupBox_5);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        textEdit_2 = new QTextEdit(groupBox_5);
        textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));

        gridLayout_3->addWidget(textEdit_2, 0, 0, 1, 1);

        splitter_2->addWidget(groupBox_5);
        splitter_3->addWidget(splitter_2);
        splitter_4->addWidget(splitter_3);

        gridLayout_5->addWidget(splitter_4, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 890, 25));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu->menuAction());
        menu->addAction(actionabout);
        toolBar->addAction(actConnect);
        toolBar->addAction(actDisconnect);
        toolBar->addAction(actUpload);
        toolBar->addAction(actDownload);
        toolBar->addAction(actDelete);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actConnect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
#if QT_CONFIG(tooltip)
        actConnect->setToolTip(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
#endif // QT_CONFIG(tooltip)
        actDisconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
#if QT_CONFIG(tooltip)
        actDisconnect->setToolTip(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
#endif // QT_CONFIG(tooltip)
        actUpload->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240\346\226\207\344\273\266", nullptr));
        actDownload->setText(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275\346\226\207\344\273\266", nullptr));
#if QT_CONFIG(tooltip)
        actDownload->setToolTip(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        actDelete->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
#if QT_CONFIG(tooltip)
        actDelete->setToolTip(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
#endif // QT_CONFIG(tooltip)
        actionabout->setText(QCoreApplication::translate("MainWindow", "about", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\347\233\256\345\275\225", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\345\210\227\350\241\250\346\230\276\347\244\272", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\350\257\246\347\273\206\344\277\241\346\201\257", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\345\217\260", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

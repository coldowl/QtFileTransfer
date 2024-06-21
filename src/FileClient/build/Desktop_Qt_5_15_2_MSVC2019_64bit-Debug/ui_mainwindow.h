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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
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
    QGridLayout *gridLayout_3;
    QSplitter *splitter_2;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QGridLayout *gridLayout_2;
    QSplitter *splitter;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    QTreeView *treeView;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_10;
    QLineEdit *pathLineEdit;
    QListView *listView;
    QWidget *tab_2;
    QGridLayout *gridLayout_7;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QTreeView *treeView_2;
    QPushButton *pushButton;
    QWidget *widget;
    QGridLayout *gridLayout_12;
    QSplitter *splitter_3;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_11;
    QTextEdit *datailTextEdit;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_5;
    QTextBrowser *textBrowser;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menu;
    QToolBar *toolBar;
    QToolBar *toolBar_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1043, 733);
        actConnect = new QAction(MainWindow);
        actConnect->setObjectName(QString::fromUtf8("actConnect"));
        actDisconnect = new QAction(MainWindow);
        actDisconnect->setObjectName(QString::fromUtf8("actDisconnect"));
        actDisconnect->setEnabled(false);
        actUpload = new QAction(MainWindow);
        actUpload->setObjectName(QString::fromUtf8("actUpload"));
        actUpload->setEnabled(false);
        actDownload = new QAction(MainWindow);
        actDownload->setObjectName(QString::fromUtf8("actDownload"));
        actDownload->setEnabled(false);
        actDelete = new QAction(MainWindow);
        actDelete->setObjectName(QString::fromUtf8("actDelete"));
        actDelete->setEnabled(false);
        actionabout = new QAction(MainWindow);
        actionabout->setObjectName(QString::fromUtf8("actionabout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        splitter_2 = new QSplitter(centralwidget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(splitter_2);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        gridLayout_2 = new QGridLayout(tab_1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        splitter = new QSplitter(tab_1);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        groupBox_3 = new QGroupBox(splitter);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        treeView = new QTreeView(groupBox_3);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        gridLayout_9->addWidget(treeView, 0, 0, 1, 1);

        splitter->addWidget(groupBox_3);
        groupBox_2 = new QGroupBox(splitter);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_10 = new QGridLayout(groupBox_2);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        pathLineEdit = new QLineEdit(groupBox_2);
        pathLineEdit->setObjectName(QString::fromUtf8("pathLineEdit"));

        gridLayout_10->addWidget(pathLineEdit, 0, 0, 1, 1);

        listView = new QListView(groupBox_2);
        listView->setObjectName(QString::fromUtf8("listView"));

        gridLayout_10->addWidget(listView, 1, 0, 1, 1);

        splitter->addWidget(groupBox_2);

        gridLayout_2->addWidget(splitter, 0, 0, 1, 1);

        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_7 = new QGridLayout(tab_2);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        groupBox = new QGroupBox(tab_2);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        treeView_2 = new QTreeView(groupBox);
        treeView_2->setObjectName(QString::fromUtf8("treeView_2"));

        gridLayout->addWidget(treeView_2, 0, 0, 1, 1);


        gridLayout_7->addWidget(groupBox, 1, 0, 1, 1);

        pushButton = new QPushButton(tab_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout_7->addWidget(pushButton, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        splitter_2->addWidget(tabWidget);
        widget = new QWidget(splitter_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_12 = new QGridLayout(widget);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        splitter_3 = new QSplitter(widget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Vertical);
        groupBox_4 = new QGroupBox(splitter_3);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_11 = new QGridLayout(groupBox_4);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        datailTextEdit = new QTextEdit(groupBox_4);
        datailTextEdit->setObjectName(QString::fromUtf8("datailTextEdit"));

        gridLayout_11->addWidget(datailTextEdit, 0, 0, 1, 1);

        splitter_3->addWidget(groupBox_4);
        groupBox_5 = new QGroupBox(splitter_3);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_5 = new QGridLayout(groupBox_5);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        textBrowser = new QTextBrowser(groupBox_5);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        gridLayout_5->addWidget(textBrowser, 0, 0, 1, 1);

        splitter_3->addWidget(groupBox_5);

        gridLayout_12->addWidget(splitter_3, 0, 0, 1, 1);

        splitter_2->addWidget(widget);

        gridLayout_3->addWidget(splitter_2, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1043, 25));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        toolBar_2 = new QToolBar(MainWindow);
        toolBar_2->setObjectName(QString::fromUtf8("toolBar_2"));
        toolBar_2->setEnabled(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar_2);

        menubar->addAction(menu->menuAction());
        menu->addAction(actionabout);
        toolBar_2->addAction(actConnect);
        toolBar_2->addAction(actDisconnect);
        toolBar_2->addAction(actDownload);
        toolBar_2->addAction(actUpload);
        toolBar_2->addAction(actDelete);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


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
        actDelete->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\346\226\207\344\273\266", nullptr));
#if QT_CONFIG(tooltip)
        actDelete->setToolTip(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        actionabout->setText(QCoreApplication::translate("MainWindow", "about", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\347\233\256\345\275\225", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\345\210\227\350\241\250\346\230\276\347\244\272", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\345\210\227\350\241\250", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\350\257\246\347\273\206\344\277\241\346\201\257", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\345\217\260", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        toolBar_2->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar_2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

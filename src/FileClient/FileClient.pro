QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/FileTransfer

SOURCES += \
    FileTransfer/syncfiletransferui.cpp \
    custommessagehandler.cpp \
    datapacketfactory.cpp \
    fileclient.cpp \
    FileTransfer/filetransferwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mediator.cpp \
    protocolpacketfactory.cpp \
    tcpclient.cpp \
    udpclient.cpp

HEADERS += \
    FileTransfer/syncfiletransferui.h \
    custommessagehandler.h \
    datapacketfactory.h \
    fileclient.h \
    FileTransfer/filetransferwidget.h \
    mainwindow.h \
    mediator.h \
    protocolpacketfactory.h \
    tcpclient.h \
    udpclient.h

FORMS += \
    FileTransfer/filetransferwidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

RESOURCES += \
    resource.qrc

DISTFILES +=






QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 构建工程目录
CONFIG(debug,debug|release){
    MOC_DIR = $$PWD/../../temp/$$TARGET/debug/moc
    RCC_DIR = $$PWD/../../temp/$$TARGET/debug/rcc
    UI_DIR = $$PWD/../../temp/$$TARGET/debug/ui
    OBJECTS_DIR = $$PWD/../../temp/$$TARGET/debug/obj
    DESTDIR = $$PWD/../../bin/$$TARGET/debug
}
CONFIG(release,debug|release){
    MOC_DIR = $$PWD/../../temp/$$TARGET/release/moc
    RCC_DIR = $$PWD/../../temp/$$TARGET/release/rcc
    UI_DIR = $$PWD/../../temp/$$TARGET/release/ui
    OBJECTS_DIR = $$PWD/../../temp/$$TARGET/release/obj
    DESTDIR = $$PWD/../../bin/$$TARGET/release
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#在release模式下启用日志上下文信息输出
DEFINES += QT_MESSAGELOGCONTEXT

INCLUDEPATH += $$PWD/FileTransfer

SOURCES += \
    FileTransfer/syncfiletransferui.cpp \
    datapacketfactory.cpp \
    fileclient.cpp \
    FileTransfer/filetransferwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mediator.cpp \
    protocolpacketfactory.cpp \
    tcpclient.cpp \
    treeviewdelegate.cpp \
    udpclient.cpp

HEADERS += \
    FileTransfer/syncfiletransferui.h \
    datapacketfactory.h \
    fileclient.h \
    FileTransfer/filetransferwidget.h \
    mainwindow.h \
    mediator.h \
    protocolpacketfactory.h \
    savelog.h \
    tcpclient.h \
    treeviewdelegate.h \
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







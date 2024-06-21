#ifndef CUSTOMMESSAGEHANDLER_H
#define CUSTOMMESSAGEHANDLER_H

#include <QTextBrowser>
#include <QDebug>
#include <QMutex>

extern QTextBrowser* G_TextBrowser; // 全局指针，用于访问QTextBrowser

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // CUSTOMMESSAGEHANDLER_H



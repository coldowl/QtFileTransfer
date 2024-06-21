#include "custommessagehandler.h"

QTextBrowser* G_TextBrowser = nullptr; // 定义全局指针

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        text = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        text = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        text = QString("Fatal: %1").arg(msg);
        abort();
    }

    if (G_TextBrowser) {
        G_TextBrowser->append(text);
    } else {
        fprintf(stderr, "%s\n", text.toLocal8Bit().constData());
    }
}

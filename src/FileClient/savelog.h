#ifndef SAVELOG_H
#define SAVELOG_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>

class SaveLog : public QObject {
    Q_OBJECT
public:
    static SaveLog* Instance() {
        static SaveLog instance;
        return &instance;
    }

    void save(const QString &message) {
        QMutexLocker locker(&mutex);
        if (!logFile.isOpen()) {
            QString logDirPath = QDir::currentPath() + "/log";
            if (!QDir().exists(logDirPath)) {
                if (!QDir().mkdir(logDirPath)) {
                    return;
                }
            }

            logFile.setFileName(logDirPath + "/log.txt");
            if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
                return;
            } else {
            }
        }
        QTextStream out(&logFile);
        out << message << "\n";
    }

    void start() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        qInstallMessageHandler(Log);
#else
        qInstallMsgHandler(Log);
#endif
    }

    void stop() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        qInstallMessageHandler(nullptr);
#else
        qInstallMsgHandler(nullptr);
#endif
    }

    bool getUseContext() const { return useContext; }
    void setUseContext(bool value) { useContext = value; }

private:
    SaveLog() : useContext(true) {
        QString logDirPath = QDir::currentPath() + "/log";
        if (!QDir().exists(logDirPath)) {
            QDir().mkdir(logDirPath);
        }
    }
    ~SaveLog() {
        if (logFile.isOpen()) {
            logFile.close();
        }
    }

    static void Log(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        QString content;

        switch (type) {
        case QtDebugMsg:
            content = QString("[DEBUG] %1").arg(msg);
            break;
        case QtWarningMsg:
            content = QString("[WARNING] %1").arg(msg);
            break;
        case QtCriticalMsg:
            content = QString("[CRITICAL] %1").arg(msg);
            break;
        case QtFatalMsg:
            content = QString("[FATAL] %1").arg(msg);
            abort();
        case QtInfoMsg:
            break;
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (SaveLog::Instance()->getUseContext()) {
            int line = context.line;
            QString file = context.file;
            QString function = context.function;
            if (line > 0) {
                content = QString("行号: %1  文件: %2  函数: %3\n%4").arg(line).arg(file).arg(function).arg(content);
            }
        }
#endif

        SaveLog::Instance()->save(content);
    }

    QFile logFile;
    QMutex mutex;
    bool useContext;
};

#endif // SAVELOG_H

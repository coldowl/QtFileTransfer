#ifndef SYNCFILETRANSFERUI_H
#define SYNCFILETRANSFERUI_H

#include <QElapsedTimer>
#include <QObject>
#include <QQueue>

class SyncFileTransferUI : public QObject
{
    Q_OBJECT
public:
    explicit SyncFileTransferUI(QObject *parent = nullptr);

    // 更新基本信息
    void updateFileInfo(const QByteArray &info);

    // 更新进度条
    void updateProgress(qint64 bytesSent);

signals:
    void fileInfoUpdated(const QByteArray &info);
    void progressUpdated(int progress);
    void restFileSizeUpdated(qint64 size);
    void speedUpdated(double speed);
    void finishTimeUpdated(double time);

public slots:
    void onTimeStarted(const QElapsedTimer &timer);

private:

    QElapsedTimer m_timer;
    QString m_fileName = nullptr;
    qint64 m_totalFileSize = 0;
    qint64 m_previousBytesSent = 0;
    qint64 m_previousElapsedTime = 0;
    // qint64 m_bytesent;

};

#endif // SYNCFILETRANSFERUI_H

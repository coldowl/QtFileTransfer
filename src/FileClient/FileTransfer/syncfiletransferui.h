#ifndef SYNCFILETRANSFERUI_H
#define SYNCFILETRANSFERUI_H

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

private:

    QString m_fileName = nullptr;
    qint64 m_totalFileSize = 0;
    // qint64 m_bytesent;

    QQueue <qint64> m_bytesQueue;
};

#endif // SYNCFILETRANSFERUI_H

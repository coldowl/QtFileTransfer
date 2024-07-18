#ifndef SYNCFILETRANSFERUI_H
#define SYNCFILETRANSFERUI_H

#include <QObject>

class SyncFileTransferUI : public QObject
{
    Q_OBJECT
public:
    explicit SyncFileTransferUI(QObject *parent = nullptr);

    // 更新进度条
    void updateProgress(const QByteArray &info);
    void updateProgress(int progress);

signals:
    void progressValue(int progress);
};

#endif // SYNCFILETRANSFERUI_H

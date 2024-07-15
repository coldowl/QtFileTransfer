#ifndef SYNCFILETRANSFERUI_H
#define SYNCFILETRANSFERUI_H

#include <QObject>

class SyncFileTransferUI : public QObject
{
    Q_OBJECT
public:
    explicit SyncFileTransferUI(QObject *parent = nullptr);

signals:
};

#endif // SYNCFILETRANSFERUI_H

#include "syncfiletransferui.h"
#include <QDataStream>

SyncFileTransferUI::SyncFileTransferUI(QObject *parent)
    : QObject(parent){

}

void SyncFileTransferUI::updateProgress(const QByteArray &info){
    QDataStream in(info);

    int progress = 0;
    qint64 currentBytes = 0;
    qint64 totalBytes = 0;

    in >> currentBytes >> totalBytes;

    if (totalBytes > 0){
        progress = static_cast<int>((currentBytes * 100) / totalBytes);
    }
    emit progressValue(progress);
}

void SyncFileTransferUI::updateProgress(int progress){

}

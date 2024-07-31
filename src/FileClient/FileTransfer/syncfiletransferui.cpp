#include "syncfiletransferui.h"
#include "qdebug.h"
#include <QDataStream>
#include <QElapsedTimer>
#include <QTimer>

SyncFileTransferUI::SyncFileTransferUI(QObject *parent)
    : QObject(parent){

}



// 更新基础信息
void SyncFileTransferUI::updateFileInfo(const QByteArray &info){
    QDataStream in(info);

    in >> m_fileName >> m_totalFileSize;

    emit fileInfoUpdated(info);
}


// 更新进度信息
void SyncFileTransferUI::updateProgress(qint64 bytesSent) {
    if(bytesSent <= 0){
        qDebug() << "已发送的比特小于0";
        return;
    }
    qint64 currentBytesSent = bytesSent;
    qint64 currentElapsedTime = m_timer.elapsed();

    if((currentElapsedTime - m_previousElapsedTime) > 20){ // 约60hz，
        // 计算进度
        int progress = 0;
        if (m_totalFileSize > 0) {
            progress = static_cast<int>((currentBytesSent * 100) / m_totalFileSize);
        }
        emit progressUpdated(progress);

        // 计算剩余文件的大小
        qint64 restFileSize = (currentBytesSent > 0) ? (m_totalFileSize - currentBytesSent) : 0;
        emit restFileSizeUpdated(restFileSize);

        // 计算传输速度（字节/秒）
        qint64 bytesDiff = currentBytesSent - m_previousBytesSent;
        qint64 timeDiff = currentElapsedTime - m_previousElapsedTime;
        double speed = (bytesDiff * 1000.0) / timeDiff; // 字节/秒
        emit speedUpdated(speed);

        // 计算预期下载完成的时间（秒）
        double finishTime = (speed > 0) ? static_cast<double>(restFileSize) / speed : -1;
        emit finishTimeUpdated(finishTime);

        m_previousBytesSent = currentBytesSent;
        m_previousElapsedTime = currentElapsedTime;
    }


    // 文件传输完成时设置进度条为100%
    if (bytesSent >= m_totalFileSize) {
        emit progressUpdated(100);
        emit speedUpdated(0);
        emit restFileSizeUpdated(0);
        emit finishTimeUpdated(0);
    }
}

void SyncFileTransferUI::onTimeStarted(const QElapsedTimer &timer)
{
    m_timer = timer;
}



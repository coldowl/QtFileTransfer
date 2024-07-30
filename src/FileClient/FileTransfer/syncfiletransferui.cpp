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
    m_bytesQueue.enqueue(bytesSent);

    static qint64 previousBytesSent = 0;
    static QTimer *timer = nullptr;
    static bool timerStarted = false;

    if (!timerStarted) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            if (m_bytesQueue.isEmpty())
                return;

            qint64 currentBytesSent = m_bytesQueue.last();

            // 计算进度
            int progress = 0;
            if (m_totalFileSize > 0) {
                progress = static_cast<int>((currentBytesSent * 100) / m_totalFileSize);
            }
            emit progressUpdated(progress);

            // 计算剩余文件的大小
            qint64 restFileSize = m_totalFileSize - currentBytesSent;
            emit restFileSizeUpdated(restFileSize);

            // 计算传输速度（字节/秒）
            qint64 bytesDiff = currentBytesSent - previousBytesSent;
            double speed = (bytesDiff * 1000.0) / 5; // 字节/秒
            emit speedUpdated(speed);
            // qDebug() << "<previousBytesSent>" << previousBytesSent << "<currentBytesSent>" << currentBytesSent;
            // qDebug() << speed;

            // 计算预期下载完成的时间（秒）
            double finishTime = (speed > 0) ? static_cast<double>(restFileSize) / speed : -1;
            emit finishTimeUpdated(finishTime);

            previousBytesSent = currentBytesSent;

            // 如果文件传输完成，停止定时器
            if (currentBytesSent >= m_totalFileSize) {
                timer->stop();
                timer->deleteLater();
            }
        });
        timer->start(5); // 每5毫秒执行一次
        timerStarted = true;
    }

    // 文件传输完成时设置进度条为100%
    if (bytesSent >= m_totalFileSize) {
        emit progressUpdated(100);
        emit speedUpdated(0);
        emit restFileSizeUpdated(0);
        emit finishTimeUpdated(0);

        // 确保定时器停止
        if (timer) {
            timer->stop();
            timer->deleteLater();
            timer = nullptr;
            timerStarted = false; // 允许在下一次传输时重新启动定时器
        }
    }
}



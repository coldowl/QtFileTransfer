#ifndef FILETRANSFERWIDGET_H
#define FILETRANSFERWIDGET_H

#include <QWidget>
// #include "syncfiletransferui.h"

namespace Ui {
class FileTransferWidget;
}

class FileTransferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransferWidget(QWidget *parent = nullptr);
    ~FileTransferWidget();

public slots:
    // 设置文件基本信息
    void setFileInfo(const QByteArray &info);

    // 设置传输进度信息
    void setProgress(int progress);
    void setPartiesName(const bool isupload);
    void setRestFileSize(qint64 size);
    void setSpeed(double speed);
    void setFinishTime(double time);

private slots:
    void on_pcButton_clicked();

    void on_cancelButton_clicked();

signals:
    void pauseTransfer();
    void resumeTransfer();

private:
    Ui::FileTransferWidget *ui;
    QString m_fileName = nullptr;
    qint64 m_totalFileSize = 0;
    QString m_totalFileSizeStr = nullptr;
    bool m_isPause;
    // SyncFileTransferUI *m_syncFileFileTransferUI = nullptr;

};

#endif // FILETRANSFERWIDGET_H

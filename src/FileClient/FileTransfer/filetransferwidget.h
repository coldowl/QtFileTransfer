#ifndef FILETRANSFERWIDGET_H
#define FILETRANSFERWIDGET_H

#include <QWidget>
#include "fileclient.h"

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
    void setBasicInfo(const QByteArray &info);

    // 设置传输进度信息
    void setProgress(int progress);

    // 设置传输记录
    void setRecordInfo(const QByteArray &info);

private:
    Ui::FileTransferWidget *ui;
    FileClient *m_fileClient;
};

#endif // FILETRANSFERWIDGET_H

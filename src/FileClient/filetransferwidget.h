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

private slots:
    void setFileInfo(QDataStream &in);

private:
    Ui::FileTransferWidget *ui;
    FileClient *m_fileClient;
};

#endif // FILETRANSFERWIDGET_H

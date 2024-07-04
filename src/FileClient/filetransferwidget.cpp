#include "filetransferwidget.h"
#include "ui_filetransferwidget.h"
#include "fileclient.h"

FileTransferWidget::FileTransferWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::FileTransferWidget)
{
    ui->setupUi(this);
    // FileClient *fileClient = new FileClient(this);
    // connect(fileClient, &FileClient::uploadInfo, this, &FileTransferWidget::setFileInfo);
}

FileTransferWidget::~FileTransferWidget()
{
    delete ui;
}

void FileTransferWidget::setFileInfo(QDataStream &in){

    QString fileName;
    qint64 fileSize;
    QString fileSatus;

    in >> fileName >> fileSize >> fileSatus;

    // 将 fileSize 转换为 QString 并进行拼接
    QString fileSizeString = QString::number(fileSize);
    ui->fileNameLabel->setText(fileName + fileSizeString);
    ui->statusLabel->setText(fileSatus);


}

#include "filetransferwidget.h"
#include "ui_filetransferwidget.h"
#include <QStringListModel>
#include <QDateTime>

FileTransferWidget::FileTransferWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::FileTransferWidget)
{
    ui->setupUi(this);
    // FileClient *fileClient = new FileClient(this);

    // 加载QSS文件
    QFile file(":/qss/filetransferwidget.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "打不开style.qss";
    }


}

FileTransferWidget::~FileTransferWidget()
{
    delete ui;
}

// 设置传输文件的基本信息
void FileTransferWidget::setBasicInfo(const QByteArray &info){
    QDataStream in(info);

    QString fileName;
    qint64 fileSize;
    QString fileSatus;

    in >> fileName >> fileSize >> fileSatus;

    QLocale locale = this->locale();
    QString betterSize = locale.formattedDataSize(fileSize);

    // 将 fileSize 转换为 QString 并进行拼接
    QString fileSizeString = QString::number(fileSize);
    ui->fileNameLabel->setText(fileName + "("+ betterSize +")");
    ui->statusLabel->setText(fileSatus);

}


// 设置进度
void FileTransferWidget::setProgress(int progress){

    ui->progressBar->setValue(progress);

    // // 创建一个 QStringListModel
    // QStringListModel *model = new QStringListModel;

    // // 创建一个字符串列表并设置为模型的数据
    // QStringList list;
    // list << "Item 1" << "Item 2" << "Item 3" << "Item 4" << QString::number(currentBytes) << QString::number(totalBytes);
    // model->setStringList(list);

    // // 将模型设置到 QListView 中
    // ui->listView->setModel(model);

}


// 设置传输记录
void FileTransferWidget::setRecordInfo(const QByteArray &info){
    QDataStream in(info);
    QString currentFileName;


    in >> currentFileName ;

    // 创建一个 QStringListModel
    QStringListModel *model = new QStringListModel;

    // 获取当前时间
    QDateTime datetime = QDateTime::currentDateTime();
    QString currentTime = datetime.toString("yyyy-MM-dd hh:mm:ss");

    // 创建一个字符串列表并设置为模型的数据
    QStringList list;

    list << currentTime + currentFileName + "传输已完成";
    model->setStringList(list);

    // 将模型设置到 QListView 中
    ui->listView->setModel(model);

}

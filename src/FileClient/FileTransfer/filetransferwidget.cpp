#include "filetransferwidget.h"
#include "ui_filetransferwidget.h"
// #include "syncfiletransferui.h"
// #include "fileclient.h"
#include <QFile>
#include <QDebug>
#include <QStringListModel>
#include <QDateTime>
#include <QIcon>
#include <QStyle>

FileTransferWidget::FileTransferWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::FileTransferWidget)
{
    ui->setupUi(this);

    // 加载QSS文件
    QFile file(":/qss/filetransferwidget.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "打不开style.qss";
    }

    // const QIcon pauseIcon = QApplication::style()->standardIcon(QStyle::SP_MediaPause);
    // const QIcon cancelIcon = QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton);
    // ui->pcButton->setIcon(pauseIcon);
    // ui->pcButton->setText("暂停");
    // ui->cancelButton->setIcon(cancelIcon);
    this->setWindowIcon(QIcon(":/icons/send.png"));
    m_isPause = false;  // 当前未暂停

    setProgress(0);

}

FileTransferWidget::~FileTransferWidget()
{
    delete ui;
}

// 在尚未开始传送之前就能获取到的信息
void FileTransferWidget::setFileInfo(const QByteArray &info){
    QDataStream in(info);

    in >> m_fileName >> m_totalFileSize;

    QLocale locale = this->locale();
    m_totalFileSizeStr = locale.formattedDataSize(m_totalFileSize);

    ui->fileNameLabel->setText(m_fileName);
    ui->fileNameLabel_2->setText(m_fileName);
    ui->restFileLabel->setText("0 Bytes/" + m_totalFileSizeStr);
    ui->stateLabel->setText("");

}

// 设置发送方和接收方
void FileTransferWidget::setPartiesName(const bool isupload){
    if(isupload){
        ui->senderLabel->setText("本地");
        ui->receiverLabel->setText("服务器");
    }else{
        ui->senderLabel->setText("服务器");
        ui->receiverLabel->setText("本地");
    }
}




// 设置进度
void FileTransferWidget::setProgress(int progress){

    ui->progressBar->setValue(progress);
    QString str = "已完成 %1 %";
    this->setWindowTitle(str.arg(progress));
    ui->stateLabel_2->setText(str.arg(progress));

}

void FileTransferWidget::setRestFileSize(qint64 size){

    QLocale locale = this->locale();
    QString restFileSizeStr = locale.formattedDataSize(size);
    ui->restFileLabel->setText(restFileSizeStr);

}

void FileTransferWidget::setSpeed(double speed){
    QLocale locale = this->locale();
    QString speedStr = locale.formattedDataSize(speed);
    ui->speedLabel->setText(speedStr+"/s");

}

void FileTransferWidget::setFinishTime(double time){
    QString finishTimeStr = QString::number(time);
    ui->restTimeLabel->setText(finishTimeStr+" 秒");
}

// // 设置传输记录
// void FileTransferWidget::setRecordInfo(const QByteArray &info){
//     QDataStream in(info);
//     QString currentFileName;


//     in >> currentFileName ;

//     // 创建一个 QStringListModel
//     QStringListModel *model = new QStringListModel;

//     // 获取当前时间
//     QDateTime datetime = QDateTime::currentDateTime();
//     QString currentTime = datetime.toString("yyyy-MM-dd hh:mm:ss");

//     // 创建一个字符串列表并设置为模型的数据
//     QStringList list;

//     list << currentTime + currentFileName + "传输已完成";
//     model->setStringList(list);

//     // 将模型设置到 QListView 中
//     ui->listView->setModel(model);

// }


// 暂停(继续)按钮
void FileTransferWidget::on_pcButton_clicked(){
    if(!m_isPause){
        ui->pcButton->setStyleSheet("image:url(:/icons/play.png)");
        ui->stateLabel->setText("已暂停 - ");
        m_isPause = true;
    }else{
        ui->pcButton->setStyleSheet("image:url(:/icons/pause.png)");
        ui->stateLabel->setText("");
        m_isPause = false;
    }
}

void FileTransferWidget::on_cancelButton_clicked(){

}



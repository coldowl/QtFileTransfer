#include "treeviewform.h"
#include "ui_treeviewform.h"
#include <QDir>
#include <QFileSystemModel>
#include <QDebug>
#include <QToolButton>


TreeViewForm::TreeViewForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TreeViewForm)
{
    ui->setupUi(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 地址栏
    addressBar = new QHBoxLayout();
    mainLayout->addLayout(addressBar);

    // 文件系统模型
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::rootPath());

    // 创建 QTreeView 控件
    treeView = new QTreeView(this);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::rootPath()));
    mainLayout->addWidget(treeView);

    connect(treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        updateAddressBar(fileSystemModel->filePath(index));
    });
    connect(fileSystemModel, &QFileSystemModel::directoryLoaded, this, &TreeViewForm::updateAddressBar);


    // 加载QSS文件
    QFile file(":/qss/treeview.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        treeView->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Could not open style.qss";
    }

    // 初始化地址栏
    updateAddressBar(fileSystemModel->filePath(treeView->rootIndex()));


}

TreeViewForm::~TreeViewForm()
{
    delete ui;
}


void TreeViewForm::updateAddressBar(const QString &path)
{
    // 清空当前地址栏
    QLayoutItem *child;
    while ((child = addressBar->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QStringList pathParts = path.split(QDir::separator());

    QString currentPath;
    for (const QString &part : pathParts) {
        if (part.isEmpty())
            continue;

        currentPath += part + QDir::separator();
        QToolButton *button = new QToolButton(this);
        button->setText(part);
        button->setProperty("path", currentPath);
        addressBar->addWidget(button);

        connect(button, &QToolButton::clicked, this, [this, currentPath] {
            treeView->setRootIndex(fileSystemModel->index(currentPath));
            updateAddressBar(currentPath);
        });
    }

    addressBar->addStretch();
}


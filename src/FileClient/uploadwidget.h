#ifndef UPLOADWIDGET_H
#define UPLOADWIDGET_H

#include <QWidget>

namespace Ui {
class UploadWidget;
}

class UploadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UploadWidget(QWidget *parent = nullptr);
    ~UploadWidget();

private:
    Ui::UploadWidget *ui;
};

#endif // UPLOADWIDGET_H

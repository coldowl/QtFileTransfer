#include "treeviewdelegate.h"
#include <QApplication>

void TreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QItemDelegate::paint(painter, option, index);

    // 获取按钮大小
    QSize buttonSize(30, 20);

    // 设置下载按钮位置
    QRect downloadRect = option.rect;
    downloadRect.setLeft(downloadRect.right() - buttonSize.width() - 5);
    downloadRect.setWidth(buttonSize.width());
    downloadRect.setHeight(buttonSize.height());

    // 设置删除按钮位置
    QRect deleteRect = option.rect;
    deleteRect.setLeft(downloadRect.left() - buttonSize.width() - 5);
    deleteRect.setWidth(buttonSize.width());
    deleteRect.setHeight(buttonSize.height());

    // 绘制按钮
    QStyleOptionButton buttonOption;
    buttonOption.rect = downloadRect;
    buttonOption.text = "下载";
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, option.widget);

    buttonOption.rect = deleteRect;
    buttonOption.text = "删除";
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, option.widget);
}

bool TreeViewDelegate::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        // 检查点击的位置是否在按钮区域内
        // ...
        // if (/* 点击在下载按钮上 */) {
        //     // 执行下载操作
        //     // ...
        // } else if (/* 点击在删除按钮上 */) {
        //     // 执行删除操作
        //     // ...
        // }
    }
    return QItemDelegate::eventFilter(obj, event);
}

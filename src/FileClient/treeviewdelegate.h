#ifndef TREEVIEWDELEGATE_H
#define TREEVIEWDELEGATE_H

#include <QItemDelegate>
#include <QMouseEvent>
#include <QPushButton>

class TreeViewDelegate : public QItemDelegate {
    Q_OBJECT

public:
    TreeViewDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
         // ... 私有成员变量和函数 ...
};
#endif // TREEVIEWDELEGATE_H

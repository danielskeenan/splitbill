/**
 * @file BillLineDelegate.h
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#ifndef SPLITBILL_SRC_UI_BILLLINEDELEGATE_H_
#define SPLITBILL_SRC_UI_BILLLINEDELEGATE_H_

#include <QtWidgets/QStyledItemDelegate>

namespace splitbill::ui {

class BillLineDelegate : public QStyledItemDelegate {
 Q_OBJECT
 public:
  explicit BillLineDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_BILLLINEDELEGATE_H_

/**
 * @file PersonListDelegate.h
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#ifndef SPLITBILL_SRC_UI_PERSONLISTDELEGATE_H_
#define SPLITBILL_SRC_UI_PERSONLISTDELEGATE_H_

#include <QtWidgets/QStyledItemDelegate>

namespace splitbill::ui {

/**
 * Delegate for Person List
 */
class PersonListDelegate : public QStyledItemDelegate {
 Q_OBJECT
 public:
  explicit PersonListDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

  [[nodiscard]] QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_PERSONLISTDELEGATE_H_

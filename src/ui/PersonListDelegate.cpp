/**
 * @file PersonListDelegate.cpp
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#include <QtWidgets/QDateEdit>
#include "PersonListDelegate.h"
#include "PersonListModel.h"

namespace splitbill::ui {

QWidget *PersonListDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const {
  if (index.column() == PersonListModel::Column::START || index.column() == PersonListModel::Column::END) {
    auto date_entry = new QDateEdit(QDate::currentDate(), parent);
    date_entry->setFrame(false);
    date_entry->setCalendarPopup(true);
    return date_entry;
  }

  return QStyledItemDelegate::createEditor(parent, option, index);
}

void PersonListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  QVariant value = index.model()->data(index, Qt::ItemDataRole::EditRole);
  if (!value.isValid()) {
    return;
  }

  if (index.column() == PersonListModel::Column::START || index.column() == PersonListModel::Column::END) {
    auto *date_entry = dynamic_cast<QDateEdit *>(editor);
    date_entry->setDate(value.toDate());
  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void PersonListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  if (index.column() == PersonListModel::Column::START || index.column() == PersonListModel::Column::END) {
    auto *date_entry = dynamic_cast<QDateEdit *>(editor);
    model->setData(index, date_entry->date());
  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}

void PersonListDelegate::updateEditorGeometry(QWidget *editor,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
} // splitbill::ui

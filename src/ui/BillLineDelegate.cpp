/**
 * @file BillLineDelegate.cpp
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>
#include "trans.h"
#include "BillLineDelegate.h"
#include "BillLineModel.h"

namespace splitbill::ui {

QWidget *BillLineDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  if (index.column() == BillLineModel::Column::AMOUNT) {
    auto *amount_entry = new QDoubleSpinBox(parent);
    amount_entry->setFrame(false);
    amount_entry->setPrefix(_("Bill line edit amount prefix", "$"));
    amount_entry->setSuffix(_("Bill line edit amount suffix", ""));
    amount_entry->setMinimum(0);
    amount_entry->setMaximum(999.99);
    amount_entry->setDecimals(2);
    return amount_entry;
  } else if (index.column() == BillLineModel::Column::TAX_RATE) {
    auto *tax_entry = new QDoubleSpinBox(parent);
    tax_entry->setFrame(false);
    tax_entry->setSuffix(_("Tax rate suffix", "%"));
    tax_entry->setMinimum(0);
    tax_entry->setMaximum(100);
    tax_entry->setDecimals(3);
    return tax_entry;
  } else if (index.column() == BillLineModel::Column::IS_SPLIT) {
    auto *is_split_entry = new QCheckBox(parent);
    is_split_entry->setTristate(false);
    return is_split_entry;
  }

  return QStyledItemDelegate::createEditor(parent, option, index);
}

void BillLineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  QVariant value = index.model()->data(index, Qt::ItemDataRole::EditRole);
  if (!value.isValid()) {
    return;
  }

  if (index.column() == BillLineModel::Column::AMOUNT) {
    auto *spin_box = dynamic_cast<QDoubleSpinBox *>(editor);
    spin_box->setValue(value.toDouble());
  } else if (index.column() == BillLineModel::Column::TAX_RATE) {
    auto *spin_box = dynamic_cast<QDoubleSpinBox *>(editor);
    // Display as a percentage
    spin_box->setValue(value.toDouble() * 100);
  } else if (index.column() == BillLineModel::Column::IS_SPLIT) {
    auto *check_box = dynamic_cast<QCheckBox *>(editor);
    check_box->setChecked(value.toBool());
  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void BillLineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  if (index.column() == BillLineModel::Column::AMOUNT) {
    auto *spin_box = dynamic_cast<QDoubleSpinBox *>(editor);
    model->setData(index, spin_box->value());
  } else if (index.column() == BillLineModel::Column::TAX_RATE) {
    auto *spin_box = dynamic_cast<QDoubleSpinBox *>(editor);
    // Displayed as a percentage
    model->setData(index, spin_box->value() / 100);
  } else if (index.column() == BillLineModel::Column::IS_SPLIT) {
    auto *check_box = dynamic_cast<QCheckBox *>(editor);
    model->setData(index, check_box->isChecked());
  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}

void BillLineDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}

} // splitbill::ui

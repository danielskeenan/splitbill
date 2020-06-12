/**
 * @file BillLineModel.cpp
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#include <QtCore/QSet>
#include "BillLineModel.h"
#include "trans.h"

namespace splitbill::ui {

BillLineModel::BillLineModel(QSharedPointer<Bill> bill, QObject *parent) :
    QAbstractTableModel(parent), bill_(bill) {
}

int BillLineModel::rowCount(const QModelIndex &parent) const {
  return bill_->GetLineCount();
}

int BillLineModel::columnCount(const QModelIndex &parent) const {
  return COLUMN_COUNT;
}

Qt::ItemFlags BillLineModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren
      | Qt::ItemFlag::ItemIsSelectable;
  if (index.column() == Column::IS_SPLIT) {
    // Checkboxes are technically not editable, but are checkable.
    flags |= Qt::ItemFlag::ItemIsUserCheckable;
  } else {
    flags |= Qt::ItemFlag::ItemIsEditable;
  }
  return flags;
}

#define COL_HEADER_LABEL(col, label) case col: return _("Bill line table column header", label)

QVariant BillLineModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Vertical) {
      // Row number
      return section + 1;
    }

    switch (section) {
      COL_HEADER_LABEL(Column::NAME, "Name");
      COL_HEADER_LABEL(Column::DESCRIPTION, "Description");
      COL_HEADER_LABEL(Column::AMOUNT, "Amount");
      COL_HEADER_LABEL(Column::TAX_RATE, "Tax");
      COL_HEADER_LABEL(Column::IS_SPLIT, "Usage");
      default: return QVariant();
    }
  }

  return QVariant();
}

QVariant BillLineModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  if (role == Qt::ItemDataRole::DisplayRole) {
    BillLine line = bill_->GetLine(index.row());
    if (index.column() == Column::NAME) {
      return QString::fromStdString(line.name);
    } else if (index.column() == Column::DESCRIPTION) {
      return QString::fromStdString(line.description);
    } else if (index.column() == Column::AMOUNT) {
      std::stringstream string_val;
      string_val << boost::locale::as::currency << line.amount.convert_to<double>();
      return QString::fromStdString(string_val.str());
    } else if (index.column() == Column::TAX_RATE) {
      std::stringstream string_val;
      string_val << boost::locale::as::percent << line.tax_rate.convert_to<double>();
      return QString::fromStdString(string_val.str());
    } else if (index.column() == Column::IS_SPLIT) {
      return line.split ? _("Bill line usage", "Yes") : _("Bill line usage", "No");
    }
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    BillLine line = bill_->GetLine(index.row());
    if (index.column() == Column::IS_SPLIT) {
      return line.split ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    }
  } else if (role == Qt::ItemDataRole::EditRole) {
    BillLine line = bill_->GetLine(index.row());
    if (index.column() == Column::NAME) {
      return QString::fromStdString(line.name);
    } else if (index.column() == Column::DESCRIPTION) {
      return QString::fromStdString(line.description);
    } else if (index.column() == Column::AMOUNT) {
      return line.amount.convert_to<double>();
    } else if (index.column() == Column::TAX_RATE) {
      return line.tax_rate.convert_to<double>();
    } else if (index.column() == Column::IS_SPLIT) {
      return line.split;
    }
  }

  return QVariant();
}

bool BillLineModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid()) {
    return false;
  }
  if (role == Qt::ItemDataRole::EditRole) {
    BillLine line = bill_->GetLine(index.row());
    switch (index.column()) {
      case Column::NAME:line.name = value.toString().toStdString();
        break;
      case Column::DESCRIPTION: line.description = value.toString().toStdString();
        break;
      case Column::AMOUNT: line.amount = value.toDouble();
        break;
      case Column::TAX_RATE: line.tax_rate = value.toDouble();
        break;
      case Column::IS_SPLIT: line.split = value.toBool();
        break;
      default:return false;
    }
    bill_->UpdateLine(index.row(), line);
    emit(dataChanged(index, index));
    return true;
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    BillLine line = bill_->GetLine(index.row());
    if (index.column() == Column::IS_SPLIT) {
      auto checked = static_cast<Qt::CheckState>(value.toULongLong());
      line.split = (checked == Qt::CheckState::Checked);
    } else {
      return false;
    }
    bill_->UpdateLine(index.row(), line);
    emit(dataChanged(index, index));
    return true;
  }

  return false;
}

void BillLineModel::AddLine(const BillLine &line) {
  const QModelIndex parent;
  const int pos = rowCount(parent);
  beginInsertRows(parent, pos, pos);
  bill_->AddLine(line);
  endInsertRows();
}

void BillLineModel::AddLine(const QModelIndex &index) {
  const QModelIndex parent;
  BillLine line;

  if (index.isValid()) {
    beginInsertRows(parent, index.row(), index.row());
    bill_->AddLine(line, index.row());
  } else {
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    bill_->AddLine(line);
  }
  endInsertRows();
}

void BillLineModel::RemoveLine(const BillLine &line) {
  for (size_t i = 0; i < bill_->GetLineCount(); i++) {
    if (bill_->GetLine(i) == line) {
      RemoveLine(i);
      return;
    }
  }
  throw std::out_of_range("Tried to remove a line that is not in the bill.");
}

void BillLineModel::RemoveLine(const size_t &pos) {
  const QModelIndex parent;
  beginRemoveRows(parent, pos, pos);
  bill_->RemoveLine(pos);
  endRemoveRows();
}

void BillLineModel::RemoveLine(const QModelIndex &index) {
  if (index.isValid()) {
    RemoveLine(index.row());
  }
}

void BillLineModel::RemoveLines(const QModelIndexList &indexes) {
  // Get the rows affected
  QSet<int> selected_rows;
  for (const auto &index : indexes) {
    if (index.isValid()) {
      selected_rows.insert(index.row());
    }
  }
  if (selected_rows.empty()) {
    return;
  }

  QList<int> row_list = selected_rows.values();
  // Reverse sort the list to remove the last affected row first.  This keeps indexes consistent.
  std::sort(row_list.begin(), row_list.end(), std::greater<>());
  const QModelIndex parent;
  const int first_row = row_list.last();
  const int last_row = row_list.first();
  beginRemoveRows(parent, first_row, last_row);
  for (const auto &row : selected_rows) {
    bill_->RemoveLine(row);
  }
  endRemoveRows();
}

} // splitbill::ui

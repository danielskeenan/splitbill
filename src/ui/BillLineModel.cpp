/**
 * @file BillLineModel.cpp
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#include <QtCore/QSet>
#include <utility>
#include "BillLineModel.h"
#include "Settings.h"

namespace splitbill::ui {

const std::unordered_map<BillLineModel::Column, QString> BillLineModel::kColumnNames{
    {Column::kName, tr("Name")},
    {Column::kDescription, tr("Description")},
    {Column::kAmount, tr("Amount")},
    {Column::kTaxRate, tr("Tax")},
    {Column::kIsSplit, tr("Usage")},
};

BillLineModel::BillLineModel(QSharedPointer<Bill> bill, QObject *parent) :
    QAbstractTableModel(parent), bill_(std::move(bill)) {
}

int BillLineModel::rowCount(const QModelIndex &parent) const {
  return bill_->GetLineCount();
}

int BillLineModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

Qt::ItemFlags BillLineModel::flags(const QModelIndex &index) const {
  const auto column = static_cast<Column>(index.column());
  Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren
      | Qt::ItemFlag::ItemIsSelectable;
  if (column == Column::kIsSplit) {
    flags |= Qt::ItemFlag::ItemIsUserCheckable;
  } else {
    flags |= Qt::ItemFlag::ItemIsEditable;
  }
  return flags;
}

QVariant BillLineModel::headerData(int section, Qt::Orientation orientation, int role) const {
  const auto column = static_cast<Column>(section);
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Vertical) {
      // Row number
      return section + 1;
    }
    return kColumnNames.at(column);
  }

  return {};
}

QVariant BillLineModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const BillLine &line = bill_->GetLine(index.row());

  if (role == Qt::ItemDataRole::DisplayRole) {
    if (column == Column::kName) {
      return QString::fromStdString(line.name);
    } else if (column == Column::kDescription) {
      return QString::fromStdString(line.description);
    } else if (column == Column::kAmount) {
      return QLocale().toCurrencyString(line.amount.GetValue());
    } else if (column == Column::kTaxRate) {
      return QLocale().toString(line.tax_rate, 'f', 3) + QLocale().percent();
    } else if (column == Column::kIsSplit) {
      //: Bill line usage
      return line.split ? tr("Yes") : tr("No");
    }
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    if (column == Column::kIsSplit) {
      return line.split ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    }
  } else if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kName) {
      return QString::fromStdString(line.name);
    } else if (column == Column::kDescription) {
      return QString::fromStdString(line.description);
    } else if (column == Column::kAmount) {
      return line.amount.GetValue();
    } else if (column == Column::kTaxRate) {
      return line.tax_rate;
    } else if (column == Column::kIsSplit) {
      return line.split;
    }
  }

  return {};
}

bool BillLineModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());
  BillLine line = bill_->GetLine(index.row());
  bool success = false;

  if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kName) {
      line.name = value.toString().toStdString();
      success = true;
    } else if (column == Column::kDescription) {
      line.description = value.toString().toStdString();
      success = true;
    } else if (column == Column::kAmount) {
      line.amount = Money(value.toDouble(&success), QLocale().currencySymbol(QLocale::CurrencyIsoCode).toStdString());
    } else if (column == Column::kTaxRate) {
      line.tax_rate = value.toDouble(&success);
    } else if (column == Column::kIsSplit) {
      line.split = value.toBool();
      success = true;
    }
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    if (column == Column::kIsSplit) {
      auto checked = value.value<Qt::CheckState>();
      line.split = (checked == Qt::CheckState::Checked);
      success = true;
    }
  }

  if (success) {
    bill_->UpdateLine(index.row(), line);
    Q_EMIT(dataChanged(index, index));
  }
  return success;
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
  BillLine line(QLocale().currencySymbol(QLocale::CurrencyIsoCode).toStdString());
  line.tax_rate = Settings::GetDefaultTaxRate();

  if (index.isValid()) {
    // Add at specific position
    beginInsertRows(parent, index.row(), index.row());
    bill_->AddLine(line, index.row());
  } else {
    // Add at end
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    bill_->AddLine(line);
  }
  endInsertRows();
}

void BillLineModel::RemoveLine(const BillLine &line) {
  // Need to know where the line is to emit the proper signal
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

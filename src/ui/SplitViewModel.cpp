/**
 * @file SplitViewModel.cpp
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#include "SplitViewModel.h"
#include <QtCore/QVector>
#include <QLocale>

namespace splitbill::ui {

const std::unordered_map<SplitViewModel::Column, QString> SplitViewModel::kColumnNames{
    {Column::kName, tr("Name")},
    {Column::kTotal, tr("Total")},
};

int SplitViewModel::rowCount(const QModelIndex &parent) const {
  return bill_portions_.size();
}

int SplitViewModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant SplitViewModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const BillPortion &portion = bill_portions_.at(index.row());

  if (role == Qt::ItemDataRole::DisplayRole) {
    if (column == Column::kName) {
      return QString::fromStdString(portion.GetName());
    } else if (column == Column::kTotal) {
      return QLocale().toCurrencyString(portion.GetTotal());
    }
  }

  return {};
}

QVariant SplitViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
  const auto column = static_cast<Column>(section);
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Horizontal) {
      return kColumnNames.at(column);
    }
  }

  return {};
}

void SplitViewModel::Update(const QDate &start, const QDate &end, const QVector<PersonPeriod> &people_periods) {
  if (people_periods.empty()) {
    return;
  }

  QVector<std::string> people;
  for (const auto &period : people_periods) {
    if (!people.contains(period.GetName())) {
      people.append(period.GetName());
    }
  }

  // Get the new bill portions
  std::vector new_portions = bill_->Split(
      start.toString(Qt::DateFormat::ISODate).toStdString(),
      end.toString(Qt::DateFormat::ISODate).toStdString(),
      std::vector<PersonPeriod>(people_periods.cbegin(), people_periods.cend()),
      std::vector<std::string>(people.cbegin(), people.cend())
  );

  // Update the data representation
  beginResetModel();
  bill_portions_ = std::move(new_portions);
  endResetModel();
}

} // splitbill::ui

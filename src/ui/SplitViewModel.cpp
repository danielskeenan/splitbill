/**
 * @file SplitViewModel.cpp
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#include "SplitViewModel.h"
#include <QtCore/QVector>
#include "trans.h"

namespace splitbill::ui {

int SplitViewModel::rowCount(const QModelIndex &parent) const {
  return bill_portions_.size();
}

int SplitViewModel::columnCount(const QModelIndex &parent) const {
  return COLUMN_COUNT;
}

QVariant SplitViewModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  if (role == Qt::ItemDataRole::DisplayRole) {
    BillPortion portion = bill_portions_.at(index.row());
    if (index.column() == Column::NAME) {
      return QString::fromStdString(portion.GetName());
//    } else if (index.column() == Column::GENERAL) {
//      std::stringstream general_str;
//      general_str << boost::locale::as::currency << portion.GetGeneralTotal();
//      return QString::fromStdString(general_str.str());
//    } else if (index.column() == Column::USAGE) {
//      std::stringstream usage_str;
//      usage_str << boost::locale::as::currency << portion.GetUsageTotal();
//      return QString::fromStdString(usage_str.str());
    } else if (index.column() == Column::TOTAL) {
      std::stringstream total_str;
      total_str << boost::locale::as::currency << portion.GetTotal();
      return QString::fromStdString(total_str.str());
    }
  }

  return QVariant();
}

#define COL_HEADER_LABEL(col, label) case col: return _("Split view column header", label)

QVariant SplitViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Horizontal) {
      switch (section) {
        COL_HEADER_LABEL(Column::NAME, "Name");
//        COL_HEADER_LABEL(Column::GENERAL, "General");
//        COL_HEADER_LABEL(Column::USAGE, "Usage");
        COL_HEADER_LABEL(Column::TOTAL, "Total");
      }
    }
  }

  return QVariant();
}

void SplitViewModel::Update(const QDate &start, const QDate &end, QVector<PersonPeriod> people_periods) {
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

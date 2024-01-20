/**
 * @file PersonListModel.cpp
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#include <QtCore/QSet>
#include <QtCore/QDate>
#include <utility>
#include "PersonListModel.h"

namespace splitbill::ui {

const std::unordered_map<PersonListModel::Column, QString> PersonListModel::kColumnNames{
    {Column::kName, tr("Name")},
    {Column::kStart, tr("From")},
    {Column::kEnd, tr("To")},
};

PersonListModel::PersonListModel(QSharedPointer<QVector<PersonPeriod>> people, QObject *parent) :
    QAbstractTableModel(parent), people_(std::move(people)) {

}

int PersonListModel::rowCount(const QModelIndex &parent) const {
  return people_->count();
}

int PersonListModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

Qt::ItemFlags PersonListModel::flags(const QModelIndex &index) const {
  return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren
      | Qt::ItemFlag::ItemIsEditable;
}

QVariant PersonListModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const PersonPeriod &person = people_->at(index.row());
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (column == Column::kName) {
      return QString::fromStdString(person.GetName());
    } else if (column == Column::kStart) {
      const QDate start_date(person.GetPeriod().begin().year(),
                             person.GetPeriod().begin().month(),
                             person.GetPeriod().begin().day());
      return QLocale().toString(start_date, QLocale::ShortFormat);
    } else if (column == Column::kEnd) {
      const QDate end_date(person.GetPeriod().end().year(),
                           person.GetPeriod().end().month(),
                           person.GetPeriod().end().day());
      return QLocale().toString(end_date, QLocale::ShortFormat);
    }
  } else if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kName) {
      return QString::fromStdString(person.GetName());
    } else if (column == Column::kStart) {
      return QDate(person.GetPeriod().begin().year(),
                   person.GetPeriod().begin().month(),
                   person.GetPeriod().begin().day());
    } else if (column == Column::kEnd) {
      return QDate(person.GetPeriod().end().year(),
                   person.GetPeriod().end().month(),
                   person.GetPeriod().end().day());
    }
  }

  return {};
}

bool PersonListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());
  PersonPeriod person = people_->at(index.row());
  bool success = false;

  if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kName) {
      person.SetName(value.toString().toStdString());
      success = true;
    } else if (column == Column::kStart) {
      person.SetStart(value.toDate().toString(Qt::DateFormat::ISODate).toStdString());
      success = true;
    } else if (column == Column::kEnd) {
      person.SetEnd(value.toDate().toString(Qt::DateFormat::ISODate).toStdString());
      success = true;
    }
  }

  if (success) {
    people_->replace(index.row(), person);
    Q_EMIT(dataChanged(index, index));
  }

  return success;
}

QVariant PersonListModel::headerData(int section, Qt::Orientation orientation, int role) const {
  const auto column = static_cast<Column>(section);
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Horizontal) {
      return kColumnNames.at(column);
    }
  }

  return {};
}

void PersonListModel::AddLine(const PersonPeriod &person_period, const QModelIndex &index) {
  const QModelIndex parent;

  if (index.isValid()) {
    // Insert at specific position
    beginInsertRows(parent, index.row(), index.row());
    people_->insert(index.row(), person_period);
  } else {
    // Insert at end
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    people_->append(person_period);
  }
  endInsertRows();
}

void PersonListModel::RemoveLine(const size_t &pos) {
  const QModelIndex parent;
  beginRemoveRows(parent, pos, pos);
  people_->remove(pos);
  endRemoveRows();
}

void PersonListModel::RemoveLine(const QModelIndex &index) {
  if (index.isValid()) {
    RemoveLine(index.row());
  }
}

void PersonListModel::RemoveLines(const QModelIndexList &indexes) {
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
    people_->remove(row);
  }
  endRemoveRows();
}

} // splitbill::ui

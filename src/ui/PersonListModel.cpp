/**
 * @file PersonListModel.cpp
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#include <QtCore/QSet>
#include <QtCore/QDate>
#include "PersonListModel.h"

namespace splitbill::ui {

PersonListModel::PersonListModel(QSharedPointer<QVector<PersonPeriod>> people, QObject *parent) :
    QAbstractTableModel(parent), people_(people) {

}

int PersonListModel::rowCount(const QModelIndex &parent) const {
  return people_->count();
}

int PersonListModel::columnCount(const QModelIndex &parent) const {
  return COLUMN_COUNT;
}

Qt::ItemFlags PersonListModel::flags(const QModelIndex &index) const {
  return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren
      | Qt::ItemFlag::ItemIsEditable;
}

QVariant PersonListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  const PersonPeriod &person = people_->at(index.row());
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (index.column() == Column::NAME) {
      return QString::fromStdString(person.GetName());
    } else if (index.column() == Column::START) {
      const QDate start_date(person.GetPeriod().begin().year(),
                             person.GetPeriod().begin().month(),
                             person.GetPeriod().begin().day());
      return QLocale().toString(start_date, QLocale::ShortFormat);
    } else if (index.column() == Column::END) {
      const QDate end_date(person.GetPeriod().end().year(),
                           person.GetPeriod().end().month(),
                           person.GetPeriod().end().day());
      return QLocale().toString(end_date, QLocale::ShortFormat);
    }
  } else if (role == Qt::ItemDataRole::EditRole) {
    if (index.column() == Column::NAME) {
      return QString::fromStdString(person.GetName());
    } else if (index.column() == Column::START) {
      return QDate::fromString(QString::fromStdString(person.GetStart()), Qt::DateFormat::ISODate);
    } else if (index.column() == Column::END) {
      return QDate::fromString(QString::fromStdString(person.GetEnd()), Qt::DateFormat::ISODate);
    }
  }

  return QVariant();
}

bool PersonListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid()) {
    return false;
  }
  if (role == Qt::ItemDataRole::EditRole) {
    PersonPeriod person = people_->at(index.row());
    if (index.column() == Column::NAME) {
      person.SetName(value.toString().toStdString());
    } else if (index.column() == Column::START) {
      person.SetStart(value.toDate().toString(Qt::DateFormat::ISODate).toStdString());
    } else if (index.column() == Column::END) {
      person.SetEnd(value.toDate().toString(Qt::DateFormat::ISODate).toStdString());
    }
    people_->replace(index.row(), person);
    emit dataChanged(index, index);
    return true;
  }

  return false;
}

#define COL_HEADER_LABEL(col, label) case col: return _("Person table column header", label)

QVariant PersonListModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::ItemDataRole::DisplayRole) {
    if (orientation == Qt::Orientation::Horizontal) {
      switch (section) {
        case Column::NAME:return tr("Name");
        case Column::START:return tr("From");
        case Column::END:return tr("To");
        default: return QVariant();
      }
    }
  }

  return QVariant();
}

void PersonListModel::AddLine(const PersonPeriod &person_period, const QModelIndex &index) {
  const QModelIndex parent;

  if (index.isValid()) {
    beginInsertRows(parent, index.row(), index.row());
    people_->insert(index.row(), person_period);
  } else {
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

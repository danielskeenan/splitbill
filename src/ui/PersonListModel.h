/**
 * @file PersonListModel.h
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#ifndef SPLITBILL_SRC_UI_PERSONLISTMODEL_H_
#define SPLITBILL_SRC_UI_PERSONLISTMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSharedPointer>
#include <unordered_map>
#include <lib/Bill.h>
#include "PersonListDelegate.h"

namespace splitbill::ui {

/**
 * Model for the person list
 */
class PersonListModel : public QAbstractTableModel {
  friend class PersonListDelegate;

 Q_OBJECT
 public:
  explicit PersonListModel(QSharedPointer<QVector<PersonPeriod>> people, QObject *parent);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void AddLine(const PersonPeriod &person_period, const QModelIndex &index);
  void RemoveLine(const size_t &pos);
  void RemoveLine(const QModelIndex &index);
  void RemoveLines(const QModelIndexList &indexes);

 private:
  QSharedPointer<QVector<PersonPeriod>> people_;
  enum class Column {
    kName = 0,
    kStart,
    kEnd,
  };
  static const unsigned int kColumnCount = static_cast<unsigned int>(Column::kEnd) + 1;

  static const std::unordered_map<Column, QString> kColumnNames;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_PERSONLISTMODEL_H_

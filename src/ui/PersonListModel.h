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
#include "../lib/Bill.h"
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

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void AddLine(const QModelIndex &index = QModelIndex());
  void RemoveLine(const size_t &pos);
  void RemoveLine(const QModelIndex &index);
  void RemoveLines(const QModelIndexList &indexes);

 private:
  QSharedPointer<QVector<PersonPeriod>> people_;
  typedef enum : char {
    NAME = 0,
    START,
    END,
  } Column;
  static const unsigned char COLUMN_COUNT = Column::END + 1;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_PERSONLISTMODEL_H_

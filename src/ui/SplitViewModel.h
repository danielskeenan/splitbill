/**
 * @file SplitViewModel.h
 *
 * @author dankeenan 
 * @date 6/6/20
 */

#ifndef SPLITBILL_SRC_UI_SPLITVIEWMODEL_H_
#define SPLITBILL_SRC_UI_SPLITVIEWMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSharedPointer>
#include <QtCore/QDate>
#include "../lib/Bill.h"

namespace splitbill::ui {

class SplitViewModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit SplitViewModel(QSharedPointer<Bill> bill, QObject *parent) :
      QAbstractTableModel(parent), bill_(bill) {}

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void Update(const QDate &start, const QDate &end, const QVector<PersonPeriod> people_periods);

 private:
  QSharedPointer<Bill> bill_;
  std::vector<BillPortion> bill_portions_;

  typedef enum : char {
    NAME = 0,
//    GENERAL,
//    USAGE,
    TOTAL,
  } Column;
  static const unsigned char COLUMN_COUNT = Column::TOTAL + 1;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_SPLITVIEWMODEL_H_

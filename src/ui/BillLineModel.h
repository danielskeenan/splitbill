/**
 * @file BillLineModel.h
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#ifndef SPLITBILL_SRC_UI_BILLLINEMODEL_H_
#define SPLITBILL_SRC_UI_BILLLINEMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QSharedPointer>
#include "../lib/Bill.h"
#include "BillLineDelegate.h"

namespace splitbill::ui {

/**
 * Bill Line model class
 */
class BillLineModel : public QAbstractTableModel {
  friend BillLineDelegate;

 Q_OBJECT
 public:
  explicit BillLineModel(QSharedPointer<Bill> bill, QObject *parent);

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  void AddLine(const BillLine &line);
  void AddLine(const QModelIndex &index = QModelIndex());

  void RemoveLine(const BillLine &line);
  void RemoveLine(const size_t &pos);
  void RemoveLine(const QModelIndex &index);
  void RemoveLines(const QModelIndexList &indexes);

 private:
  QSharedPointer<Bill> bill_;
  typedef enum : char {
    NAME = 0,
    DESCRIPTION,
    AMOUNT,
    TAX_RATE,
    IS_SPLIT,
  } Column;
  static const unsigned char COLUMN_COUNT = Column::IS_SPLIT + 1;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_BILLLINEMODEL_H_

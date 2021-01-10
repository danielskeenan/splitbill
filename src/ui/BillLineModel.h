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
#include <unordered_map>
#include <lib/Bill.h>
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

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  void AddLine(const BillLine &line);
  void AddLine(const QModelIndex &index = QModelIndex());

  void RemoveLine(const BillLine &line);
  void RemoveLine(const size_t &pos);
  void RemoveLine(const QModelIndex &index);
  void RemoveLines(const QModelIndexList &indexes);

 private:
  QSharedPointer<Bill> bill_;
  enum class Column {
    kName = 0,
    kDescription,
    kAmount,
    kTaxRate,
    kIsSplit,
  };
  static const unsigned int kColumnCount = static_cast<unsigned int>(Column::kIsSplit) + 1;

  static const std::unordered_map<Column, QString> kColumnNames;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_BILLLINEMODEL_H_

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
#include <unordered_map>
#include <lib/Bill.h>

namespace splitbill::ui {

class SplitViewModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit SplitViewModel(QSharedPointer<Bill> bill, QObject *parent) :
      QAbstractTableModel(parent), bill_(std::move(bill)) {}

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void Update(const QDate &start, const QDate &end, const QVector<PersonPeriod> &people_periods);

 private:
  QSharedPointer<Bill> bill_;
  std::vector<BillPortion> bill_portions_;

  enum class Column {
    kName = 0,
    kTotal,
  };
  static const unsigned int kColumnCount = static_cast<unsigned int>(Column::kTotal) + 1;

  static const std::unordered_map<Column, QString> kColumnNames;
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_SPLITVIEWMODEL_H_

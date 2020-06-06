/**
 * @file MainWindow.h
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#ifndef SPLITBILL_SRC_UI_MAINWINDOW_H_
#define SPLITBILL_SRC_UI_MAINWINDOW_H_

#include <QtCore/QPointer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDateEdit>
#include "BillLineModel.h"
#include "../lib/Bill.h"
#include "PersonListModel.h"
#include "SplitViewModel.h"

namespace splitbill::ui {

/**
 * Main Window
 */
class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = nullptr);

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  struct Widgets {
    QDoubleSpinBox *billTotalEntry;
    QDateEdit *billDateStart;
    QDateEdit *billDateEnd;
    QTableView *lineView;
    QLabel *billLineTotalLabel;
    QLabel *billIsValidIcon;
    QLabel *billIsValidLabel;
    QTableView *peopleView;
    QTableView *splitView;
  };

  void InitUi();
  QWidget *InitBillOverview();
  void InitBillLineTable();
  QWidget *InitPeopleTable();
  QWidget *InitSplitTable();

  Widgets widgets_;
  QPointer<BillLineModel> bill_line_model_;
  QSharedPointer<Bill> bill_;
  QPointer<PersonListModel> person_list_model_;
  QSharedPointer<QVector<PersonPeriod>> people_;
  QPointer<SplitViewModel> split_view_model_;

 private slots:
  void s_AddBillLine();
  void s_RemoveBillLine();
  void s_AddPerson();
  void s_RemovePerson();
  void s_UpdateLineTotal();
  void s_UpdateBillTotal(double val);
  void s_UpdateBillValidation();
  void s_UpdateSplit();
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_MAINWINDOW_H_

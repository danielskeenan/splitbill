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
#include <lib/Bill.h>
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
    QDoubleSpinBox *billTotalEntry = nullptr;
    QDateEdit *billDateStart = nullptr;
    QDateEdit *billDateEnd = nullptr;
    QTableView *lineView = nullptr;
    QLabel *billLineTotalLabel = nullptr;
    QLabel *billIsValidIcon = nullptr;
    QLabel *billIsValidLabel = nullptr;
    QTableView *peopleView = nullptr;
    QTableView *splitView = nullptr;
  };
  Widgets widgets_;
  QPointer<BillLineModel> bill_line_model_;
  QSharedPointer<Bill> bill_;
  QPointer<PersonListModel> person_list_model_;
  QSharedPointer<QVector<PersonPeriod>> people_;
  QPointer<SplitViewModel> split_view_model_;

  void InitUi();
  void InitMenu();
  QWidget *InitBillOverview();
  void InitBillLineTable();
  QWidget *InitPeopleTable();
  QWidget *InitSplitTable();

 private Q_SLOTS:
  // Menu actions
  void SPreferences();
  void SAbout();

  // UI actions
  void SAddBillLine();
  void SRemoveBillLine();
  void SAddPerson();
  void SRemovePerson();
  void SUpdateLineTotal();
  void SUpdateBillTotal(double val);
  void SUpdateBillValidation();
  void SUpdateSplit();
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_MAINWINDOW_H_

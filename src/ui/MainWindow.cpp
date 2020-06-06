/**
 * @file MainWindow.cpp
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#include "MainWindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include "Settings.h"
#include "trans.h"

namespace splitbill::ui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), bill_(new Bill), people_(new QVector<PersonPeriod>) {
  InitUi();
}

void MainWindow::InitUi() {
  move(Settings::GetWindowPosition());
  resize(Settings::GetWindowSize());

  // Setup the layout
  auto *central_widget = new QWidget(this);
  setCentralWidget(central_widget);
  auto *layout = new QHBoxLayout;
  auto *splitter = new QSplitter;
  splitter->setChildrenCollapsible(false);
  layout->addWidget(splitter);
  auto *left = new QWidget(this);
  splitter->addWidget(left);
  auto *leftLayout = new QVBoxLayout;
  left->setLayout(leftLayout);
  auto *right = new QWidget(this);
//  right->setMinimumWidth(400);
  splitter->addWidget(right);
  auto *rightLayout = new QVBoxLayout;
  right->setLayout(rightLayout);
  central_widget->setLayout(layout);

  // Bill Line list
  InitBillLineTable();
  leftLayout->addWidget(widgets_.lineView);

  // Bill line total
  auto line_info_layout = new QHBoxLayout;
  leftLayout->addLayout(line_info_layout);
  line_info_layout->addWidget(new QLabel(_("Bill line total label", "Line Total:")));
  widgets_.billLineTotalLabel = new QLabel;
  s_UpdateLineTotal();
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::s_UpdateLineTotal);
  line_info_layout->addWidget(widgets_.billLineTotalLabel);

  // Add/Remove Buttons
  auto *action_buttons = new QDialogButtonBox(this);
  line_info_layout->addWidget(action_buttons);
  auto *add_button = new QPushButton(_("Bill line table", "Add Line"), this);
  add_button->setIcon(QIcon(":/add"));
  connect(add_button, &QPushButton::clicked, this, &MainWindow::s_AddBillLine);
  action_buttons->addButton(add_button, QDialogButtonBox::ButtonRole::ActionRole);
  auto *remove_button = new QPushButton(_("Bill line table", "Remove Line"), this);
  remove_button->setIcon(QIcon(":/remove"));
  connect(remove_button, &QPushButton::clicked, this, &MainWindow::s_RemoveBillLine);
  action_buttons->addButton(remove_button, QDialogButtonBox::ButtonRole::ActionRole);

  // Bill overview
  rightLayout->addWidget(InitBillOverview());

  // Bill validation status
  auto bill_valid = new QGroupBox(_("Bill validation status label", "Validation"), this);
  rightLayout->addWidget(bill_valid);
  auto bill_valid_layout = new QHBoxLayout;
  bill_valid->setLayout(bill_valid_layout);
  bill_valid_layout->addStretch();
  widgets_.billIsValidIcon = new QLabel(this);
  bill_valid_layout->addWidget(widgets_.billIsValidIcon);
  widgets_.billIsValidLabel = new QLabel(this);
  widgets_.billIsValidLabel->setWordWrap(true);
  bill_valid_layout->addWidget(widgets_.billIsValidLabel);
  bill_valid_layout->addStretch();
  s_UpdateBillValidation();
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::s_UpdateBillValidation);
  connect(widgets_.billTotalEntry,
          QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::s_UpdateBillTotal);

  // People list
  rightLayout->addWidget(InitPeopleTable());

  // Split table
  rightLayout->addWidget(InitSplitTable());

  // Split Button
//  auto split_button = new QPushButton(_("Split action button", "Split"), this);
//  split_button->setIcon(QIcon(":/find-dollar"));
//  rightLayout->addWidget(split_button);
}

QWidget *MainWindow::InitBillOverview() {
  auto bill_overview = new QGroupBox(_("Bill overview label", "Bill Overview"), this);
  auto layout = new QFormLayout;
  bill_overview->setLayout(layout);

  // Bill total
  widgets_.billTotalEntry = new QDoubleSpinBox(this);
  widgets_.billTotalEntry->setPrefix(_("Bill line edit amount prefix", "$"));
  widgets_.billTotalEntry->setSuffix(_("Bill line edit amount suffix", ""));
  widgets_.billTotalEntry->setMinimum(0);
  widgets_.billTotalEntry->setMaximum(999.99);
  widgets_.billTotalEntry->setDecimals(2);
  layout->addRow(_("Bill total label", "Bill total"), widgets_.billTotalEntry);

  // Start date
  widgets_.billDateStart = new QDateEdit(QDate::currentDate(), this);
  widgets_.billDateStart->setCalendarPopup(true);
  connect(widgets_.billDateStart, &QDateEdit::dateChanged, this, &MainWindow::s_UpdateBillValidation);
  connect(widgets_.billDateStart, &QDateEdit::dateChanged, this, &MainWindow::s_UpdateSplit);
  layout->addRow(_("Bill start date label", "Start"), widgets_.billDateStart);

  // End date
  widgets_.billDateEnd = new QDateEdit(QDate::currentDate(), this);
  widgets_.billDateEnd->setCalendarPopup(true);
  connect(widgets_.billDateEnd, &QDateEdit::dateChanged, this, &MainWindow::s_UpdateBillValidation);
  connect(widgets_.billDateEnd, &QDateEdit::dateChanged, this, &MainWindow::s_UpdateSplit);
  layout->addRow(_("Bill end date label", "End"), widgets_.billDateEnd);

  return bill_overview;
}

void MainWindow::InitBillLineTable() {
  widgets_.lineView = new QTableView(this);
  widgets_.lineView->setMinimumWidth(600);
  widgets_.lineView->setSelectionMode(QTableView::SelectionMode::ExtendedSelection);

  bill_line_model_ = new BillLineModel(bill_, this);
  widgets_.lineView->setModel(bill_line_model_);
  auto *bill_line_delegate = new BillLineDelegate(this);
  widgets_.lineView->setItemDelegate(bill_line_delegate);
  connect(bill_line_model_, &BillLineModel::rowsInserted, this, &MainWindow::s_UpdateSplit);
  connect(bill_line_model_, &BillLineModel::rowsRemoved, this, &MainWindow::s_UpdateSplit);
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::s_UpdateSplit);
}

QWidget *MainWindow::InitPeopleTable() {
  auto people_list = new QGroupBox(_("People list label", "People"), this);
  people_list->setMinimumWidth(300);
  auto people_layout = new QVBoxLayout;
  people_list->setLayout(people_layout);
  widgets_.peopleView = new QTableView(this);
  widgets_.peopleView->setSelectionMode(QTableView::SelectionMode::ExtendedSelection);
  person_list_model_ = new PersonListModel(people_, this);
  widgets_.peopleView->setModel(person_list_model_);
  people_layout->addWidget(widgets_.peopleView);
  connect(person_list_model_, &PersonListModel::rowsInserted, this, &MainWindow::s_UpdateSplit);
  connect(person_list_model_, &PersonListModel::rowsRemoved, this, &MainWindow::s_UpdateSplit);
  connect(person_list_model_, &PersonListModel::dataChanged, this, &MainWindow::s_UpdateSplit);

  // Add/Remove Buttons
  auto *action_buttons = new QDialogButtonBox(this);
  people_layout->addWidget(action_buttons);
  auto *add_button = new QPushButton(_("People table", "Add Person"), this);
  add_button->setIcon(QIcon(":/add"));
  connect(add_button, &QPushButton::clicked, this, &MainWindow::s_AddPerson);
  action_buttons->addButton(add_button, QDialogButtonBox::ButtonRole::ActionRole);
  auto *remove_button = new QPushButton(_("People table", "Remove Person"), this);
  remove_button->setIcon(QIcon(":/remove"));
  connect(remove_button, &QPushButton::clicked, this, &MainWindow::s_RemovePerson);
  action_buttons->addButton(remove_button, QDialogButtonBox::ButtonRole::ActionRole);

  return people_list;
}

QWidget *MainWindow::InitSplitTable() {
  auto *split_view = new QGroupBox(_("Split table label", "Split"), this);
  auto *layout = new QVBoxLayout;
  split_view->setLayout(layout);
  widgets_.splitView = new QTableView(this);
  split_view_model_ = new SplitViewModel(bill_, this);
  widgets_.splitView->setModel(split_view_model_);
  layout->addWidget(widgets_.splitView);
  s_UpdateSplit();

  return split_view;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  Settings::SetWindowPosition(pos());
  Settings::SetWindowSize(size());
  QWidget::closeEvent(event);
}

void MainWindow::s_AddBillLine() {
  QItemSelectionModel *selection = widgets_.lineView->selectionModel();
  QModelIndex selected = selection->currentIndex();
  bill_line_model_->AddLine(selected);
}

void MainWindow::s_RemoveBillLine() {
  bill_line_model_->RemoveLines(widgets_.lineView->selectionModel()->selectedIndexes());
}

void MainWindow::s_AddPerson() {
  QItemSelectionModel *selection = widgets_.peopleView->selectionModel();
  QModelIndex selected = selection->currentIndex();
  person_list_model_->AddLine(selected);
}

void MainWindow::s_RemovePerson() {
  person_list_model_->RemoveLines(widgets_.peopleView->selectionModel()->selectedIndexes());
}

void MainWindow::s_UpdateLineTotal() {
  SplitBill totals = bill_->Total();
  std::stringstream total_str;
  total_str << boost::locale::as::currency << totals.GetTotal();
  widgets_.billLineTotalLabel->setText(QString::fromStdString(total_str.str()));
}

void MainWindow::s_UpdateBillTotal(double val) {
  bill_->SetTotalAmount(val);
  s_UpdateBillValidation();
  s_UpdateSplit();
}

void MainWindow::s_UpdateBillValidation() {
  static const QSize icon_size = QSize(16, 16);
  ValidationError error;
  if (!bill_->IsValid(error)) {
    if (error == ValidationError::LINE_SUM_NOT_TOTAL) {
      widgets_.billIsValidLabel->setText(_("Bill validator error", "The sum of the lines does not equal the total."));
    } else {
      widgets_.billIsValidLabel->setText(_("Bill validator error", "The bill is not valid for an unknown reason."));
    }
    widgets_.billIsValidIcon->setPixmap(QIcon(":/bad").pixmap(icon_size));
  } else if (widgets_.billDateStart->date() > widgets_.billDateEnd->date()) {
    // The main bill doesn't care about start/end dates
    widgets_.billIsValidLabel->setText(_("Bill validator error", "The billing period ends before it starts."));
    widgets_.billIsValidIcon->setPixmap(QIcon(":/bad").pixmap(icon_size));
  } else {
    widgets_.billIsValidLabel->setText(_("Bill validator error", "The bill is valid."));
    widgets_.billIsValidIcon->setPixmap(QIcon(":/good").pixmap(icon_size));
  }
}

void MainWindow::s_UpdateSplit() {
  ValidationError error;
  if (bill_->IsValid(error) && widgets_.billDateStart->date() <= widgets_.billDateEnd->date()) {
    split_view_model_->Update(widgets_.billDateStart->date(), widgets_.billDateEnd->date(), *people_);
  }
}

} // splitbill::ui

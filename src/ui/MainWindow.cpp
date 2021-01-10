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
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QCloseEvent>
#include <QApplication>
#include <QMessageBox>
#include "Settings.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

namespace splitbill::ui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), bill_(new Bill), people_(new QVector<PersonPeriod>) {
  InitUi();
  InitMenu();
}

void MainWindow::InitUi() {
  if (!restoreGeometry(Settings::GetMainWindowGeometry())) {
    resize(600, 600);
  }

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
  //: Bill line total label
  line_info_layout->addWidget(new QLabel(tr("Line Total:")));
  widgets_.billLineTotalLabel = new QLabel;
  SUpdateLineTotal();
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::SUpdateLineTotal);
  line_info_layout->addWidget(widgets_.billLineTotalLabel);

  // Add/Remove Buttons
  auto *action_buttons = new QDialogButtonBox(this);
  line_info_layout->addWidget(action_buttons);
  //: Bill line table
  auto *add_button = new QPushButton(tr("Add Line"), this);
  add_button->setIcon(QIcon::fromTheme("list-add"));
  connect(add_button, &QPushButton::clicked, this, &MainWindow::SAddBillLine);
  action_buttons->addButton(add_button, QDialogButtonBox::ButtonRole::ActionRole);
  //: Bill line table
  auto *remove_button = new QPushButton(tr("Remove Line"), this);
  remove_button->setIcon(QIcon::fromTheme("list-remove"));
  connect(remove_button, &QPushButton::clicked, this, &MainWindow::SRemoveBillLine);
  action_buttons->addButton(remove_button, QDialogButtonBox::ButtonRole::ActionRole);

  // Bill overview
  rightLayout->addWidget(InitBillOverview());

  // Bill validation status
  auto bill_valid = new QGroupBox(tr("Validation"), this);
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
  SUpdateBillValidation();
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::SUpdateBillValidation);
  connect(widgets_.billTotalEntry,
          QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::SUpdateBillTotal);

  // People list
  rightLayout->addWidget(InitPeopleTable());

  // Split table
  rightLayout->addWidget(InitSplitTable());
}

void MainWindow::InitMenu() {
  // Edit menu
  QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
  // Preferences
  QAction *edit_preferences = edit_menu->addAction(tr("&Preferences"));
  connect(edit_preferences, &QAction::triggered, this, &MainWindow::SPreferences);

  // Help menu
  QMenu *help_menu = menuBar()->addMenu(tr("&Help"));
  // About
  QAction *help_about = help_menu->addAction(tr("&About"));
  connect(help_about, &QAction::triggered, this, &MainWindow::SAbout);
  // About Qt
  QAction *help_about_qt = help_menu->addAction(tr("About &Qt"));
  connect(help_about_qt, &QAction::triggered, [this]() { QMessageBox::aboutQt(this); });
}

QWidget *MainWindow::InitBillOverview() {
  auto bill_overview = new QGroupBox(tr("Bill Overview"), this);
  auto layout = new QFormLayout;
  bill_overview->setLayout(layout);

  // Bill total
  widgets_.billTotalEntry = new QDoubleSpinBox(this);
  widgets_.billTotalEntry->setPrefix(QLocale().currencySymbol());
  widgets_.billTotalEntry->setMinimum(0);
  widgets_.billTotalEntry->setMaximum(999.99);
  widgets_.billTotalEntry->setDecimals(2);
  layout->addRow(tr("Bill total"), widgets_.billTotalEntry);

  // Start date
  widgets_.billDateStart = new QDateEdit(QDate::currentDate(), this);
  widgets_.billDateStart->setCalendarPopup(true);
  connect(widgets_.billDateStart, &QDateEdit::dateChanged, this, &MainWindow::SUpdateBillValidation);
  connect(widgets_.billDateStart, &QDateEdit::dateChanged, this, &MainWindow::SUpdateSplit);
  layout->addRow(tr("Start"), widgets_.billDateStart);

  // End date
  widgets_.billDateEnd = new QDateEdit(QDate::currentDate(), this);
  widgets_.billDateEnd->setCalendarPopup(true);
  connect(widgets_.billDateEnd, &QDateEdit::dateChanged, this, &MainWindow::SUpdateBillValidation);
  connect(widgets_.billDateEnd, &QDateEdit::dateChanged, this, &MainWindow::SUpdateSplit);
  layout->addRow(tr("End"), widgets_.billDateEnd);

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
  connect(bill_line_model_, &BillLineModel::rowsInserted, this, &MainWindow::SUpdateSplit);
  connect(bill_line_model_, &BillLineModel::rowsRemoved, this, &MainWindow::SUpdateSplit);
  connect(bill_line_model_, &BillLineModel::dataChanged, this, &MainWindow::SUpdateSplit);
}

QWidget *MainWindow::InitPeopleTable() {
  auto people_list = new QGroupBox(tr("People"), this);
  people_list->setMinimumWidth(350);
  auto people_layout = new QVBoxLayout;
  people_list->setLayout(people_layout);
  widgets_.peopleView = new QTableView(this);
  widgets_.peopleView->setSelectionMode(QTableView::SelectionMode::ExtendedSelection);
  for (const auto &person : Settings::GetDefaultPeople()) {
    PersonPeriod person_period(person.toStdString(),
                               widgets_.billDateStart->date().toString(Qt::DateFormat::ISODate).toStdString(),
                               widgets_.billDateEnd->date().toString(Qt::DateFormat::ISODate).toStdString());
    people_->append(person_period);
  }
  person_list_model_ = new PersonListModel(people_, this);
  widgets_.peopleView->setModel(person_list_model_);
  people_layout->addWidget(widgets_.peopleView);
  connect(person_list_model_, &PersonListModel::rowsInserted, this, &MainWindow::SUpdateSplit);
  connect(person_list_model_, &PersonListModel::rowsRemoved, this, &MainWindow::SUpdateSplit);
  connect(person_list_model_, &PersonListModel::dataChanged, this, &MainWindow::SUpdateSplit);

  // Add/Remove Buttons
  auto *action_buttons = new QDialogButtonBox(this);
  people_layout->addWidget(action_buttons);
  auto *add_button = new QPushButton(tr("Add Person"), this);
  add_button->setIcon(QIcon::fromTheme("list-add"));
  connect(add_button, &QPushButton::clicked, this, &MainWindow::SAddPerson);
  action_buttons->addButton(add_button, QDialogButtonBox::ButtonRole::ActionRole);
  auto *remove_button = new QPushButton(tr("Remove Person"), this);
  remove_button->setIcon(QIcon::fromTheme("list-remove"));
  connect(remove_button, &QPushButton::clicked, this, &MainWindow::SRemovePerson);
  action_buttons->addButton(remove_button, QDialogButtonBox::ButtonRole::ActionRole);

  return people_list;
}

QWidget *MainWindow::InitSplitTable() {
  auto *split_view = new QGroupBox(tr("Split"), this);
  auto *layout = new QVBoxLayout;
  split_view->setLayout(layout);
  widgets_.splitView = new QTableView(this);
  widgets_.splitView->setSelectionMode(QTableView::SelectionMode::SingleSelection);
  widgets_.splitView->setSelectionBehavior(QTableView::SelectionBehavior::SelectItems);
  split_view_model_ = new SplitViewModel(bill_, this);
  widgets_.splitView->setModel(split_view_model_);
  layout->addWidget(widgets_.splitView);
  SUpdateSplit();

  return split_view;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  Settings::SetMainWindowGeometry(saveGeometry());
  event->accept();
}

void MainWindow::SPreferences() {
  auto *settings_dialog = new SettingsDialog(this);
  settings_dialog->exec();
}

void MainWindow::SAbout() {
  auto *about_dialog = new AboutDialog(this);
  about_dialog->exec();
}

void MainWindow::SAddBillLine() {
  QItemSelectionModel *selection = widgets_.lineView->selectionModel();
  QModelIndex selected = selection->currentIndex();
  bill_line_model_->AddLine(selected);
}

void MainWindow::SRemoveBillLine() {
  bill_line_model_->RemoveLines(widgets_.lineView->selectionModel()->selectedIndexes());
}

void MainWindow::SAddPerson() {
  QItemSelectionModel *selection = widgets_.peopleView->selectionModel();
  QModelIndex selected = selection->currentIndex();
  PersonPeriod person_period(tr("New Person").toStdString(),
                             widgets_.billDateStart->date().toString(Qt::DateFormat::ISODate).toStdString(),
                             widgets_.billDateEnd->date().toString(Qt::DateFormat::ISODate).toStdString());
  person_list_model_->AddLine(person_period, selected);
}

void MainWindow::SRemovePerson() {
  person_list_model_->RemoveLines(widgets_.peopleView->selectionModel()->selectedIndexes());
}

void MainWindow::SUpdateLineTotal() {
  const SplitBill totals = bill_->Total();
  widgets_.billLineTotalLabel->setText(QLocale().toCurrencyString(totals.GetTotal()));
}

void MainWindow::SUpdateBillTotal(double val) {
  bill_->SetTotalAmount(val);
  SUpdateBillValidation();
  SUpdateSplit();
}

void MainWindow::SUpdateBillValidation() {
  static const QSize icon_size = QSize(16, 16);
  ValidationError error;
  if (!bill_->IsValid(error)) {
    if (error == ValidationError::kLineSumNotTotal) {
      widgets_.billIsValidLabel->setText(tr("The sum of the lines does not equal the total."));
    } else {
      widgets_.billIsValidLabel->setText(tr("The bill is not valid for an unknown reason."));
    }
    widgets_.billIsValidIcon->setPixmap(QIcon(":/bad").pixmap(icon_size));
  } else if (widgets_.billDateStart->date() > widgets_.billDateEnd->date()) {
    // The main bill doesn't care about start/end dates
    widgets_.billIsValidLabel->setText(tr("The billing period ends before it starts."));
    widgets_.billIsValidIcon->setPixmap(QIcon(":/bad").pixmap(icon_size));
  } else {
    widgets_.billIsValidLabel->setText(tr("The bill is valid."));
    widgets_.billIsValidIcon->setPixmap(QIcon(":/good").pixmap(icon_size));
  }
}

void MainWindow::SUpdateSplit() {
  ValidationError error;
  if (bill_->IsValid(error) && widgets_.billDateStart->date() <= widgets_.billDateEnd->date()) {
    split_view_model_->Update(widgets_.billDateStart->date(), widgets_.billDateEnd->date(), *people_);
  }
}

} // splitbill::ui

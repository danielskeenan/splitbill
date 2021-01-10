/**
 * @file SettingsDialog.cpp
 *
 * @author dankeenan 
 * @date 6/12/20
 */

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include "SettingsDialog.h"
#include "Settings.h"

namespace splitbill::ui {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
  InitUi();
}

void SettingsDialog::InitUi() {
  setWindowTitle(tr("Preferences"));
  auto *layout = new QVBoxLayout(this);
  auto *default_form = new QGroupBox(tr("Defaults"), this);
  auto *form_layout = new QFormLayout;
  default_form->setLayout(form_layout);
  layout->addWidget(default_form);

  // Form
  // Default tax rate
  widgets_.defaultTaxRate = new QDoubleSpinBox(this);
  widgets_.defaultTaxRate->setValue(Settings::GetDefaultTaxRate() * 100);
  widgets_.defaultTaxRate->setMinimum(0);
  widgets_.defaultTaxRate->setMaximum(100);
  widgets_.defaultTaxRate->setDecimals(3);
  widgets_.defaultTaxRate->setSuffix(QLocale().percent());
  form_layout->addRow(tr("Tax Rate"), widgets_.defaultTaxRate);
  // Default people
  auto *default_people_layout = new QVBoxLayout;
  widgets_.defaultPeople = new QListWidget(this);
  connect(widgets_.defaultPeople, &QListWidget::currentItemChanged, this, &SettingsDialog::SSelectedPersonChanged);
  widgets_.defaultPeople->addItems(Settings::GetDefaultPeople());
  default_people_layout->addWidget(widgets_.defaultPeople);
  auto *default_people_buttons = new QDialogButtonBox(this);
  auto *add_default_person = new QPushButton(tr("Add"), this);
  add_default_person->setIcon(QIcon::fromTheme("list-add"));
  connect(add_default_person, &QPushButton::clicked, this, &SettingsDialog::SAddPerson);
  default_people_buttons->addButton(add_default_person, QDialogButtonBox::ActionRole);
  auto *remove_default_person = new QPushButton(tr("Remove"), this);
  remove_default_person->setIcon(QIcon::fromTheme("list-remove"));
  connect(remove_default_person, &QPushButton::clicked, this, &SettingsDialog::SRemovePerson);
  default_people_buttons->addButton(remove_default_person, QDialogButtonBox::ActionRole);
  default_people_layout->addWidget(default_people_buttons);
  form_layout->addRow(tr("People"), default_people_layout);

  // Dialog buttons
  auto *dialog_buttons = new QDialogButtonBox(
      QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel,
      this);
  connect(dialog_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(dialog_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(dialog_buttons);
}

void SettingsDialog::SAddPerson() {
  QItemSelectionModel *selection = widgets_.defaultPeople->selectionModel();
  int index;
  if (selection->hasSelection()) {
    index = selection->currentIndex().row();
  } else {
    index = widgets_.defaultPeople->count();
  }
  auto *item = new QListWidgetItem(tr("New Person"));
  widgets_.defaultPeople->insertItem(index, item);
  widgets_.defaultPeople->setCurrentRow(index);
}

void SettingsDialog::SRemovePerson() {
  QItemSelectionModel *selection = widgets_.defaultPeople->selectionModel();
  if (selection->hasSelection()) {
    widgets_.defaultPeople->takeItem(selection->currentIndex().row());
  }
}

void SettingsDialog::SSelectedPersonChanged(QListWidgetItem *current, QListWidgetItem *previous) {
  if (current) {
    current->setFlags(current->flags() | Qt::ItemFlag::ItemIsEditable);
  }
  if (previous) {
    previous->setFlags(previous->flags() ^ Qt::ItemFlag::ItemIsEditable);
  }
}

void SettingsDialog::accept() {
  Settings::SetDefaultTaxRate(widgets_.defaultTaxRate->value() / 100);
  QStringList default_people;
  default_people.reserve(widgets_.defaultPeople->count());
  for (int i = 0; i < widgets_.defaultPeople->count(); ++i) {
    default_people.append(widgets_.defaultPeople->item(i)->data(Qt::ItemDataRole::DisplayRole).toString());
  }
  Settings::SetDefaultPeople(default_people);
}

} // splitbill::ui

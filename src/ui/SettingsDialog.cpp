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
#include "trans.h"
#include "Settings.h"

namespace splitbill::ui {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
  InitUi();
}

void SettingsDialog::InitUi() {
  setWindowTitle(_("Preferences dialog title", "Preferences"));
  auto *layout = new QVBoxLayout;
  setLayout(layout);
  auto *default_form = new QGroupBox(_("Default form", "Defaults"), this);
  auto *form_layout = new QFormLayout;
  default_form->setLayout(form_layout);
  layout->addWidget(default_form);

  // Form
  // Default tax rate
  widgets_.defaultTaxRate = new QDoubleSpinBox(this);
  widgets_.defaultTaxRate->setMinimum(0);
  widgets_.defaultTaxRate->setMaximum(100);
  widgets_.defaultTaxRate->setDecimals(3);
  widgets_.defaultTaxRate->setSuffix(_("Tax rate suffix", "%"));
  form_layout->addRow(_("Default tax rate label", "Tax Rate"), widgets_.defaultTaxRate);
  // Default people
  auto *default_people_layout = new QVBoxLayout;
  widgets_.defaultPeople = new QListWidget(this);
//  widgets_.defaultPeople->setEditTriggers(QListWidget::EditTrigger::AllEditTriggers);
  connect(widgets_.defaultPeople, &QListWidget::currentItemChanged, this, &SettingsDialog::s_SelectedPersonChanged);
  widgets_.defaultPeople->addItems(Settings::GetDefaultPeople());
  default_people_layout->addWidget(widgets_.defaultPeople);
  auto *default_people_buttons = new QDialogButtonBox(this);
  auto *add_default_person = new QPushButton(_("Add default person", "Add"), this);
  add_default_person->setIcon(QIcon(":/add"));
  connect(add_default_person, &QPushButton::clicked, this, &SettingsDialog::s_AddPerson);
  default_people_buttons->addButton(add_default_person, QDialogButtonBox::ActionRole);
  auto *remove_default_person = new QPushButton(_("Remove default person", "Remove"), this);
  remove_default_person->setIcon(QIcon(":/remove"));
  connect(remove_default_person, &QPushButton::clicked, this, &SettingsDialog::s_RemovePerson);
  default_people_buttons->addButton(remove_default_person, QDialogButtonBox::ActionRole);
  default_people_layout->addWidget(default_people_buttons);
  form_layout->addRow(_("Default people label", "People"), default_people_layout);

  // Dialog buttons
  auto *dialog_buttons = new QDialogButtonBox(
      QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel,
      this);
  connect(dialog_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(dialog_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(dialog_buttons);
}

void SettingsDialog::s_AddPerson() {
  QItemSelectionModel *selection = widgets_.defaultPeople->selectionModel();
  int index;
  if (selection->hasSelection()) {
    index = selection->currentIndex().row();
  } else {
    index = widgets_.defaultPeople->count();
  }
  auto *item = new QListWidgetItem(_("Default person name", "New Person"));
  widgets_.defaultPeople->insertItem(index, item);
  widgets_.defaultPeople->setCurrentRow(index);
}

void SettingsDialog::s_RemovePerson() {
  QItemSelectionModel *selection = widgets_.defaultPeople->selectionModel();
  if (selection->hasSelection()) {
    widgets_.defaultPeople->takeItem(selection->currentIndex().row());
  }
}

void SettingsDialog::s_SelectedPersonChanged(QListWidgetItem *current, QListWidgetItem *previous) {
  if (current) {
    current->setFlags(current->flags() | Qt::ItemFlag::ItemIsEditable);
  }
  if (previous) {
    previous->setFlags(previous->flags() ^ Qt::ItemFlag::ItemIsEditable);
  }
}

void SettingsDialog::accept() {
  Settings::SetDefaultTaxRate(widgets_.defaultTaxRate->value());
  QStringList default_people;
  default_people.reserve(widgets_.defaultPeople->count());
  for (int i = 0; i < widgets_.defaultPeople->count(); i++) {
    default_people.append(widgets_.defaultPeople->item(i)->data(Qt::ItemDataRole::DisplayRole).toString());
  }
  Settings::SetDefaultPeople(default_people);
  QDialog::accept();
}

} // splitbill::ui

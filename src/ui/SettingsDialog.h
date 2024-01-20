/**
 * @file SettingsDialog.h
 *
 * @author dankeenan 
 * @date 6/12/20
 */

#ifndef SPLITBILL_SRC_UI_SETTINGSDIALOG_H_
#define SPLITBILL_SRC_UI_SETTINGSDIALOG_H_

#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QListWidget>

namespace splitbill::ui {

class SettingsDialog : public QDialog {
 Q_OBJECT
 public:
  explicit SettingsDialog(QWidget *parent = nullptr);

 public Q_SLOTS:
  void accept() override;

 private:
  struct Widgets {
    QDoubleSpinBox *defaultTaxRate = nullptr;
    QListWidget *defaultPeople = nullptr;
  };
  Widgets widgets_;

  void InitUi();

 private Q_SLOTS:
  void SAddPerson();
  void SRemovePerson();
  void SSelectedPersonChanged(QListWidgetItem *current, QListWidgetItem *previous);
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_SETTINGSDIALOG_H_

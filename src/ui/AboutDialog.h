/**
 * @file AboutDialog.h
 *
 * @author dankeenan
 * @date 1/8/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef SPLITBILL_SRC_UI_ABOUTDIALOG_H_
#define SPLITBILL_SRC_UI_ABOUTDIALOG_H_

#include <QDialog>

namespace splitbill::ui {

/**
 * About Dialog
 */
class AboutDialog : public QDialog {
 Q_OBJECT
 public:
  explicit AboutDialog(QWidget *parent = nullptr);

 private:
  QWidget *CreateAboutTab();
  QWidget *CreateThirdPartyTab();
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_ABOUTDIALOG_H_

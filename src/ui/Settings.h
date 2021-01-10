/**
 * @file Settings.h
 *
 * @author dankeenan 
 * @date 6/5/20
 */

#ifndef SPLITBILL_SRC_UI_SETTINGS_H_
#define SPLITBILL_SRC_UI_SETTINGS_H_

#include <memory>
#include <QPoint>
#include <QSize>
#include <QtCore/QSettings>
#include <dragoonboots/qsettingscontainer/QSettingsContainer.h>

namespace splitbill::ui {

/**
 * Program settings container
 */
class Settings : public dragoonboots::qsettingscontainer::QSettingsContainer {
 public:
  DGSETTINGS_SETTING(QByteArray, MainWindowGeometry, {})

  DGSETTINGS_SETTING(double, DefaultTaxRate, 0.00)

  DGSETTINGS_SETTING(QStringList, DefaultPeople, {})
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_SETTINGS_H_

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

namespace splitbill::ui {

/**
 * Program settings container
 */
class Settings {
 private:
  struct Setting {
    constexpr static char WindowPosition[] = "position";
    constexpr static char WindowSize[] = "size";
  };

  static QVariant Get(const QString &key, const QVariant &default_value = QVariant()) {
    return QSettings().value(key, default_value);
  }

  static void Set(const QString &key, const QVariant &val) {
    QSettings().setValue(key, val);
  }

 public:
  static QPoint GetWindowPosition() {
    return Get(Setting::WindowPosition, QPoint(200, 200)).toPoint();
  }

  static void SetWindowPosition(const QPoint &pos) {
    Set(Setting::WindowPosition, pos);
  }

  static QSize GetWindowSize() {
    return Get(Setting::WindowSize, QSize(600, 600)).toSize();
  }

  static void SetWindowSize(const QSize &size) {
    Set(Setting::WindowSize, size);
  }
};

} // splitbill::ui

#endif //SPLITBILL_SRC_UI_SETTINGS_H_

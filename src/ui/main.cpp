/**
 * @file main.cpp
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#include <QtWidgets/QApplication>
#include <QIcon>
#include <QTranslator>
#include "config.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Translator
  QTranslator translator;
  translator.load(QLocale(), APP_NAME, "_", ":/i18n");
  app.installTranslator(&translator);

  // Icon theme, for non Linux platforms
#ifndef PLATFORM_LINUX
  QIcon::setThemeName("breeze-light");
#endif

  // App metadata
  app.setOrganizationName(APP_AUTHOR);
  app.setOrganizationDomain(APP_DOMAIN);
  app.setApplicationName(APP_NAME);
  app.setApplicationDisplayName("Split Bill");
  app.setApplicationVersion(APP_VERSION);
  app.setWindowIcon(QIcon(":/app-icon"));
  splitbill::ui::MainWindow main_window;
  main_window.show();

  return app.exec();
}

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
  QTranslator translator;
  translator.load(QLocale(), APP_NAME, "_", ":/i18n");
  app.installTranslator(&translator);
  app.setOrganizationName(APP_AUTHOR);
  app.setOrganizationDomain(APP_DOMAIN);
  app.setApplicationName(APP_NAME);
  app.setApplicationDisplayName(translator.translate("entrypoint", "Split Bill"));
  app.setApplicationVersion(APP_VERSION);
  app.setWindowIcon(QIcon(":/app-icon"));
  splitbill::ui::MainWindow main_window;
  main_window.show();

  return app.exec();
}

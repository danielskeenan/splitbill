/**
 * @file main.cpp
 *
 * @author dankeenan 
 * @date 6/3/20
 */

#include <QtWidgets/QApplication>
#include <QIcon>
#include "config.h"
#include "trans.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
  // Localization
  boost::locale::generator gen;
  gen.add_messages_domain(APP_NAME);
  std::locale::global(gen(""));
  std::cout.imbue(std::locale());

  // Bootstrap
  QApplication app(argc, argv);
  app.setOrganizationName(APP_AUTHOR);
  app.setOrganizationDomain(APP_DOMAIN);
  app.setApplicationName(APP_NAME);
  app.setApplicationDisplayName(_("App name", APP_DISPLAY_NAME));
  app.setApplicationVersion(APP_VERSION);
  app.setWindowIcon(QIcon(":/app-icon"));
  splitbill::ui::MainWindow main_window;
  main_window.show();

  return app.exec();
}

#include "editwindow.h"
#include "runwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include "stdio.h"


int main(int argc, char *argv[])
{
  // запрос версии
  if((argc > 1) && (QString(argv[1]).trimmed() == "-v")) {
    QTextStream stream(stdout);
    stream << QString(APP_VERSION) << '\n';
    return 0;
  }

  QApplication a(argc, argv);
  int mode = editMode;

  /** разбираем параметры **/
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

  parser.setApplicationDescription(QString("\nУправление автозапуском КСУТС v.%1")
                                   .arg(APP_VERSION));

  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();

  QList<QCommandLineOption> clo;

  QCommandLineOption editModeOption = QCommandLineOption (QStringList() << "edit",
                                 QCoreApplication::translate("main", "Работа в режиме редактора.\n"));

  QCommandLineOption statusModeOption = QCommandLineOption (QStringList() << "status",
                                 QCoreApplication::translate("main", "Режим отображения состояния.\n"));

  parser.addOption(editModeOption);
  parser.addOption(statusModeOption);

  if (parser.parse(a.arguments())) {

    if(parser.isSet(editModeOption))
      mode = editMode;

    else if(parser.isSet(statusModeOption))
      mode = statusMode;

    else
        mode = runMode;

  }

  switch (mode) {
      case editMode:
      {
          EditWindow w(mode);
          w.show();
          return a.exec();
          break;

      }
      default:
      {
          RunWindow r;
          r.show();
          return a.exec();
          break;
      }
  }

}

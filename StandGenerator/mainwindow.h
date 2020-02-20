#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QMap>

#include <QObject>

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_settings.h"

#include "sv_abstractsystem.h"
#include "oht/sv_oht.h"
#include "skm/sv_skm.h"
#include "opa/sv_opa.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
  const QString SYSTEM_OXT = "OXT";
  const QString SYSTEM_OPA = "OPA";
  const QString SYSTEM_KTV = "KTV";
  const QString SYSTEM_SKM = "SKM";
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  bool readDevices();
  
private:
  Ui::MainWindow *ui;
//  QMap<QString, AbstractSystemThread*> p_threads;
  
  QVector<QTextEdit*> p_logs;
  QVector<SvAbstractSystem*> p_systems;
  QVector<QDockWidget*> p_docks;
  
  bool constructUI();
  
  
private slots:
  void startStop(SvAbstractSystem* system);
  
};

#endif // MAINWINDOW_H

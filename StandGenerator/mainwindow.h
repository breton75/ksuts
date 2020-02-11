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
#include "sv_abstractsystem.h"
#include "sv_oht.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
  const QString SYSTEM_OHT = "OXT";
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private:
  Ui::MainWindow *ui;
//  QMap<QString, AbstractSystemThread*> p_threads;
  
  bool constructUI();
  
  bool readDevices();
  
private slots:
  void startStop(SvAbstractSystem* system);
  
};

#endif // MAINWINDOW_H

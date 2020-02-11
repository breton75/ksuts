#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_pgdb.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void on_bnLoadSystems_clicked();
  
  void on_bnSignals_clicked();
  
private:
  Ui::MainWindow *ui;
  SvException _exception;
  
  bool init();
};

#endif // MAINWINDOW_H

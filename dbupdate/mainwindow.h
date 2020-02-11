#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_busy_window.h"

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
  void on_bnOk_clicked();
  
  void on_pushButton_clicked();
  
private:
  Ui::MainWindow *ui;
  
  svlog::SvLog _log;
  
  QString _db_name;
  
  SvException _e;
  
};

#endif // MAINWINDOW_H

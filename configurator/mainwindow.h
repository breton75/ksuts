#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QDebug>
//#include <QXmlStreamReader>
//#include <QXmlReader>
//#include <QtXml>
//#include "xbelreader.h"
//#include "xbelwriter.h"
#include <QtWidgets>
//#include <QSqlError>
#include <QSql>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_pgdb.h"

#include "../global/sql_defs.h"

#include "../libs/ksutsmon/ksutsmon.h"
#include "../libs/netmon/netmon.h"
#include "../libs/aggmon/aggmon.h"

#include "treeitem.h"
#include "treemodel.h"


#include "sv_device_editor.h"
#include "sv_storage_list.h"
#include "sv_device_list.h"
#include "sv_signal_list.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  svlog::SvLog log;

  bool init();

private:
  Ui::MainWindow *ui;

  SvException _exception;

  QString _config_file_name = "config.xml";

  QString _config_path;

  QMenu *fileMenu;
  QMenu *configMenu;
  QMenu *helpMenu;

  QToolBar* toolBar;

  QAction *actionOpen;
  QAction *actionSaveAs;
  QAction *actionExit;
  QAction *aboutAct;
  QAction *aboutQtAct;

  QAction *actionEditDeviceList;
  QAction *actionEditSignalList;
  QAction *actionEditStorages;
  QAction *actionServerState;
//  QAction *action;

  TreeModel* _model;
  TreeItem* rootItem;

  Ksutsmon* _monitor;
  Netmon* _net_monitor;
  Aggmon* _agg_monitor;

  void createActions();

  bool readConfig();
  void updateTable();

private slots:
  void editDeviceList();
  void editSignalList();
  void editStorages();
  void serverState();

public slots:
    void saveAs();
    void about();


};

#endif // MAINWINDOW_H

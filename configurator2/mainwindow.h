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
#include <QApplication>
#include <QProcess>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_widget_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_config.h"
#include "../../svlib/sv_busy_window.h"
#include "../../svlib/sv_dbus.h"
#include "../../svlib/sv_abstract_logger.h"

#include "../global/sql_defs.h"
#include "../global/autorun_defs.h"
#include "../global/global_defs.h"

#include "../libs/ksutsmon/ksutsmon.h"
#include "../libs/netmon/netmon.h"
#include "../libs/aggmon/aggmon.h"
#include "../libs/soegmon/soegmon.h"

#include "treeitem.h"
#include "treemodel.h"

#include "sv_device_editor.h"
#include "sv_storage_editor.h"
#include "sv_storage_list.h"
#include "sv_device_list.h"
#include "sv_signal_list.h"

#include "edit_autorun.h"
#include "sv_editconfig.h"
#include "sv_device_log.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

//  sv::SvDBus& _dbus = sv::SvDBus::instance();


public:
  explicit MainWindow(const AppConfig& cfg, QWidget *parent = 0);
  ~MainWindow();

  sv::SvWidgetLogger mainlog;

  bool init();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
  Ui::MainWindow *ui;

  SvException _exception;

  AppConfig _config;

  QString p_current_path = "";

  QString _config_file_name = "config.xml";
  QString _config_path;

  QMenu *fileMenu;
  QMenu *configMenu;
  QMenu *serviceMenu;
  QMenu *helpMenu;

  QToolBar* toolBar;

  QAction *actionExit;
  QAction *aboutAct;
  QAction *aboutQtAct;

  QAction *actionAuth;
  QAction *actionEditConfig;
  QAction *actionEditAutorun;
  QAction *actionSaveConfig;
  QAction *actionLoadConfig;

  QAction *actionBackupDatabase;
  QAction *actionRestoreDatabase;

//  QAction *action;
//  QAction *action;

  QAction *actionEditDeviceList;
  QAction *actionEditSignalList;
  QAction *actionEditStorages;
//  QAction *actionServerState;
  QAction *actionRestartKsuts;
  QAction *actionRestartAgg;

  QAction *restoreAction;
  QAction *quitAction;

  QSystemTrayIcon *p_trayIcon;
  QMenu *p_trayIconMenu;

  TreeModel* _model = nullptr;
//  TreeItem* rootItem;

  TreeItem* _standRoot;
  TreeItem* _devicesRoot;
  TreeItem* _storagesRoot;
  TreeItem* _servicesRoot;
  TreeItem* _general_info;
  TreeItem* _autostart;
  TreeItem* _ksuts_config;
  TreeItem* _ksuts_logger;


  Ksutsmon* _ksuts_monitor = nullptr;
  Netmon* _net_monitor = nullptr;
  Aggmon* _agg_monitor = nullptr;
  Soegmon* _soeg_monitor = nullptr;

  QTimer _update_info_timer;

  QString p_current_stand_info_html = "";
  QString p_current_stand_info_hint = "";

  bool p_authorized = false;

  QString p_connection_name = "PG_KSUTS_Configurator";

  QList<int> p_device_index_list = QList<int>();

  QString _current_sender = "main";

  void createActions();
  void createTrayIcon();

  void showMessage();


  bool makeTree();
  void updateTable();

  bool pourDevicesToRoot(TreeItem *rootItem);
  bool pourSignalsToDevices(TreeItem* rootItem);

  bool pourStoragesToRoot(TreeItem* rootItem);
  bool pourDevicesToStorages(TreeItem* rootItem);
  bool pourSignalsToStorages(TreeItem *rootItem);

  void make_stand_info();

  void setAuth();

  QString get_html_page(const QString& data);
  QString get_one_device_info(int index);
  QString get_all_devices_info();
  QString get_server_config();
  QString get_autostart_info();
  QString get_one_storage_info(int index);
  QString get_all_storages_info();

  QString get_select_list(const QString& query, const QString &field);

private slots:

  void editDeviceList();
  void editSignalList();
  void editStorages();

  void auth();
  void editConfig();
  void editAutorun();

  void backupDB();
  void restoreDB();

  void update_stand_info();

  void on_treeView_clicked(const QModelIndex &index);

  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void messageClicked();

  void restartKsutsServer();
  void restartAggServer();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void currentSenderMessageSlot(const QString& sender, const QString& message, const QString &type);
  void allMessagesSlot(const QString& sender, const QString& message, const QString &type);

  void on_bnEditSender_clicked();

  void on_bnClear_clicked();

public slots:
  void loadConfig();
  void saveConfigAs();
  void about();

};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvPGDB* PGDB;
//extern SvSQLITE* SQLITE;
extern SvStorageList* STORAGELIST_UI;

extern SvSignalList* SIGNALLIST_UI;
extern SvDeviceList* DEVICELIST_UI;
extern SvDeviceEditor *DEVICE_UI;
extern SvStorageEditor* STORAGEEDITOR_UI;

extern EditAutorunWindow *EDIT_AUTORUN_UI;
extern SvEditConfig* EDITCONFIG_UI;

MainWindow::MainWindow(const AppConfig &cfg, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _config(cfg)
{
  ui->setupUi(this);

  QDir::setCurrent(qApp->applicationDirPath());

  _ksuts_monitor = new Ksutsmon(ui->centralWidget);
  ui->vlayControls->addWidget(_ksuts_monitor);
  _ksuts_monitor->setVisible(true);

  _soeg_monitor = new Soegmon(ui->centralWidget);
  ui->vlayControls->addWidget(_soeg_monitor);
  _soeg_monitor->setVisible(true);

  _net_monitor = new Netmon(ui->centralWidget);
  ui->vlayControls->addWidget(_net_monitor);
  _net_monitor->setVisible(true);

  _agg_monitor = new Aggmon(ui->centralWidget);
  ui->vlayControls->addWidget(_agg_monitor);
  _agg_monitor->setVisible(true);

  ui->widgetControls->setVisible(false);

  setWindowTitle(QString("Конфигуратор пульта v.%1").arg(APP_VERSION));
  setWindowIcon(QIcon(":/signals/icons/application-default-icon_37713.ico"));

  mainlog.setTextEdit(ui->textLog);

//  ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
  ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
//  ui->treeView->setEditTriggers(QTreeView::AllEditTriggers);
  ui->treeView->setAlternatingRowColors(false);
  ui->treeView->setWordWrap(true);
  ui->treeView->setUniformRowHeights(false);
  ui->treeView->setWordWrap(true);
  ui->treeView->header()->setStretchLastSection(true);
  ui->treeView->setUpdatesEnabled(true);
  ui->treeView->setItemsExpandable(true);

  createActions();
  setAuth();
  createTrayIcon();

  connect(p_trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
  connect(p_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  QIcon icon = QIcon(":/signals/icons/pisikga_cat_animal_9421_64.png");
  p_trayIcon->setIcon(icon);
  setWindowIcon(icon);

  p_trayIcon->show();

  fileMenu = menuBar()->addMenu(tr("&Файл"));
  fileMenu->addAction(actionRestartKsuts);
//  fileMenu->addSeparator();
  fileMenu->addAction(actionRestartAgg);
  fileMenu->addSeparator();
  fileMenu->addAction(actionExit);
//  fileMenu->addSeparator();

  configMenu = menuBar()->addMenu(tr("&Конфигурация"));
  configMenu->addAction(actionEditDeviceList);
//  configMenu->addSeparator();
  configMenu->addAction(actionEditSignalList);
//  configMenu->addSeparator();
  configMenu->addAction(actionEditStorages);
  configMenu->addSeparator();
  configMenu->addAction(actionEditAutorun);
  configMenu->addAction(actionEditConfig);

  serviceMenu = menuBar()->addMenu(tr("&Сервис"));
  serviceMenu->addAction(actionLoadConfig);
  serviceMenu->addAction(actionSaveConfig);
  serviceMenu->addSeparator();
  QMenu* a = serviceMenu->addMenu("Резервное копирование");
  a->addAction(actionBackupDatabase);
  a->addSeparator();
  a->addAction(actionRestoreDatabase);

  helpMenu = menuBar()->addMenu(tr("&Помощь"));
  helpMenu->addAction(aboutQtAct);


  ui->mainToolBar->addActions(QList<QAction*>() << actionEditDeviceList);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditSignalList);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditStorages);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditAutorun);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditConfig);

  ui->toolBarAuth->addActions(QList<QAction*>() << actionAuth);
  ui->toolBarAuth->addSeparator();

  ui->toolBarConfig->addActions(QList<QAction*>() << actionSaveConfig);
  ui->toolBarConfig->addSeparator();
  ui->toolBarConfig->addActions(QList<QAction*>() << actionLoadConfig);
  ui->toolBarConfig->addSeparator();

  ui->toolBarBackup->addActions(QList<QAction*>() << actionBackupDatabase);
  ui->toolBarBackup->addSeparator();
//  ui->toolBar->addActions(QList<QAction*>() << actionRestoreDatabase);

  AppParams::loadLayout(this);

  QDBusConnection::sessionBus().connect(QString(), QString(), DBUS_SERVER_NAME, "message", this, SLOT(currentSenderMessageSlot(const QString&,const QString&,const QString&)));


//  connect(ui->treeView, &QTreeView::entered)

//  helpMenu = menuBar()->addMenu(tr("&Help"));
//  helpMenu->addAction(aboutAct);
//  helpMenu->addAction(aboutQtAct);

//  QMetaObject::connectSlotsByName(this);


}

void MainWindow::currentSenderMessageSlot(const QString& sender, const QString& message, const QString& type)
{
  if(sender == _current_sender)
    mainlog << sv::log::stringToType(type) << QString("%1").arg(message) << sv::log::endl;
}

void MainWindow::allMessagesSlot(const QString& sender, const QString& message, const QString& type)
{
  mainlog << sv::log::stringToType(type) << QString("%1").arg(message) << sv::log::endl;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (p_trayIcon->isVisible()) {

    p_authorized = false;
    setAuth();

    hide();

    event->ignore();

  }
}

MainWindow::~MainWindow()
{
  _update_info_timer.stop();

  if(PGDB) {

    delete PGDB;
//  PGDB->disconnectFromDb();
    QSqlDatabase::removeDatabase(p_connection_name);
  }

  delete _ksuts_monitor;
  delete _net_monitor;
  delete _agg_monitor;

  delete actionLoadConfig;
  delete actionSaveConfig;
  delete actionAuth;
  delete actionEditAutorun;
  delete actionEditConfig;
  delete actionExit;
//  delete aboutAct;
//  delete aboutQtAct;
  delete actionEditDeviceList;
  delete actionEditSignalList;
  delete actionEditStorages;

  if(_model)
    delete _model;

  AppParams::saveLayout(this);

  delete ui;

}

bool MainWindow::init()
{
  QSqlQuery* q = nullptr;
  QSqlError serr;


  try {

    PGDB = new SvPGDB();
    PGDB->setConnectionParams(_config.db_name, _config.db_host, _config.db_port, _config.db_user, _config.db_pass);

    serr = PGDB->connectToDB(p_connection_name);
    if(serr.type() != QSqlError::NoError)
        _exception.raise(serr.databaseText());


//    q = new QSqlQuery(PGDB->db);
//    QSqlError serr = PGDB->execSQL(SQL_SELECT_STORAGES_LIST, q);

//    QString header_captions = "";
//    if(serr.type() != QSqlError::NoError) {

//      QMessageBox::critical(this, "Ошибка", serr.text(), QMessageBox::Ok);
//      header_captions = QString(MAIN_TREE_HEADERS); //.arg("Хр.0").arg("Хр.2").arg("Хр.2");

//    }
//    else {

//      QStringList hl = QStringList();
//      while(q->next())
//        hl << q->value("storage_name").toString();

//      if(hl.count() < 3)
//        _exception.raise("В списке хранилищ меньше трех записей");

//      header_captions = QString(MAIN_TREE_HEADERS); //.arg(hl.at(0)).arg(hl.at(1)).arg(hl.at(2));

//    }
//    q->finish();
//    delete q;

    _model = new TreeModel(QString(MAIN_TREE_HEADERS).split(';'), ui->treeView);
    ui->treeView->setModel(_model);

    makeTree();
//    updateTable();

    if(_config.single_device_mode)
      _ksuts_monitor->setSingleDeviceList(p_device_index_list);

    ui->treeView->setColumnWidth(0, 38);
//    ui->treeView->setColumnWidth(1, 90);
//    ui->treeView->setColumnWidth(2, 90);
//    ui->treeView->setColumnWidth(3, 200);
//    ui->treeView->setColumnWidth(4, 90);
//    ui->treeView->setColumnWidth(5, 90);
//    ui->treeView->setColumnWidth(6, 90);

    update_stand_info();
    ui->textBrowser->setHtml(get_html_page(p_current_stand_info_html));

    connect(&_update_info_timer, &QTimer::timeout, this, &MainWindow::update_stand_info);
    _update_info_timer.start(1000);

//    ui->treeView->setCurrentIndex(_model->index(0, 0));

    return true;

  }

  catch(SvException& e) {

    if(q) {

      q->finish();
      delete q;
    }

//    PGDB->disconnectFromDb();
    delete PGDB;
    PGDB = nullptr;
    QSqlDatabase::removeDatabase(p_connection_name);

    mainlog << sv::log::mtCritical << e.error << sv::log::endl;

    return false;

  }
}

bool MainWindow::makeTree()
{

  QSqlError serr;
  QSqlQuery* q = new QSqlQuery(PGDB->db);
  int column_count = _model->rootItem()->columnCount();

  try {

    _model->clear();

    /** группа Конфигурация пульта **/
    _standRoot = _model->rootItem()->insertChildren(_model->rootItem()->childCount(), 1, column_count);
    _standRoot->parent_index = _model->rootItem()->index;
    _standRoot->is_main_row = true;
    _standRoot->item_type = itStandRoot;
    _standRoot->setData(1, "Конфигурация пульта");
//    _standRoot->setData(0, QString(" "));
    for(int i = 0; i < column_count; i++) _standRoot->setInfo(i, ItemInfo());
    _standRoot->setInfo(0, ItemInfo(itStandRootIcon, ""));


    // добавляем разделы Общие сведения, Параметры запуска, Конфигурация КСУТС сервера, Логгер КСУТС сервера
//    _general_info = _standRoot->insertChildren(_standRoot->childCount(), 1, column_count);
//    _general_info->parent_index = _standRoot->index;
//    _general_info->is_main_row = true;
//    _general_info->item_type = itStandInfo;
//    _general_info->setData(1, "Общие сведения");

    _autostart = _standRoot->insertChildren(_standRoot->childCount(), 1, column_count);
    _autostart->parent_index = _standRoot->index;
    _autostart->is_main_row = true;
    _autostart->item_type = itAutostart;
    _autostart->setData(1, "Параметры автозапуска");

    _ksuts_config = _standRoot->insertChildren(_standRoot->childCount(), 1, column_count);
    _ksuts_config->parent_index = _standRoot->index;
    _ksuts_config->is_main_row = true;
    _ksuts_config->item_type = itConfig;
    _ksuts_config->setData(1, "Конфигурация КСУТС сервера");

    _ksuts_logger = _standRoot->insertChildren(_standRoot->childCount(), 1, column_count);
    _ksuts_logger->parent_index = _standRoot->index;
    _ksuts_logger->is_main_row = true;
    _ksuts_logger->item_type = itLogger;
    _ksuts_logger->setData(1, "Логи КСУТС сервера");

    /** разделитель 1 **/
    TreeItem* div1 = _model->rootItem()->insertChildren(_model->rootItem()->childCount(), 1, column_count);
    div1->parent_index = _model->rootItem()->index;
    div1->item_type = itUndefined;
    div1->setData(1, QString(100, ' '));


    /**      группа "Устройства"      */
    _devicesRoot = _model->rootItem()->insertChildren(_model->rootItem()->childCount(), 1, column_count);
    _devicesRoot->parent_index = _model->rootItem()->index;
    _devicesRoot->is_main_row = true;
    _devicesRoot->item_type = itDevicesRoot;
    for(int i = 0; i < column_count; i++) _devicesRoot->setInfo(i, ItemInfo());
    _devicesRoot->setInfo(0, ItemInfo(itDevicesRootIcon, ""));
//    _devicesRoot->setData(0, QString(" "));
    // определяем общее кол-во и кол-во привязанных устройств для этой стойки
    serr = PGDB->execSQL(QString(SQL_SELECT_DEVICES_COUNT_STR), q);
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    q->first();

    _devicesRoot->setData(1, QString("Устройства %1").arg(q->value(0).toString()));

    q->finish();

    // заполняем список устройств
    pourDevicesToRoot(_devicesRoot);

    // заполняем список сигналов
    pourSignalsToDevices(_devicesRoot);

    /** разделитель 2 **/
    TreeItem* div2 = _model->rootItem()->insertChildren(_model->rootItem()->childCount(), 1, column_count);
    div2->parent_index = _model->rootItem()->index;
    div2->item_type = itUndefined;
    div2->setData(1, QString(100, ' '));


    /**    группа "Хранилища"       **/
    _storagesRoot = _model->rootItem()->insertChildren(_model->rootItem()->childCount(), 1, column_count);
    _storagesRoot->parent_index = _model->rootItem()->index;
    _storagesRoot->is_main_row = true;
    _storagesRoot->item_type = itStoragesRoot;
    _storagesRoot->setData(1, QString("Хранилища"));
//    _storagesRoot->setData(0, QString(" "));
    for(int i = 0; i < column_count; i++) _storagesRoot->setInfo(i, ItemInfo());
    _storagesRoot->setInfo(0, ItemInfo(itStoragesRootIcon, ""));


    // читаем все! хранилища
    pourStoragesToRoot(_storagesRoot);

    // заполняем список устройств
    pourDevicesToStorages(_storagesRoot);

    // сигналы
    pourSignalsToStorages(_storagesRoot);


    delete q;

    ui->treeView->expandToDepth(0);

    return true;

  }

  catch(SvException& e) {

    q->finish();
    delete q;

    mainlog << sv::log::Time << sv::log::mtCritical << e.error << sv::log::endl;

    return false;

  }

}

bool MainWindow::pourDevicesToRoot(TreeItem* rootItem)
{
    QSqlError serr;
    QSqlQuery* q = new QSqlQuery(PGDB->db);
    int column_count = rootItem->columnCount();

    try {

        serr = PGDB->execSQL(SQL_SELECT_INVOLVED_DEVICES, q);
        if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

        // заполняем модель выбранными данными
        while(q->next()) {

          TreeItem *device = rootItem->insertChildren(rootItem->childCount(), 1, column_count);

          device->index = q->value("device_index").toInt();
          device->parent_index = rootItem->index;
          device->is_main_row = true;
          device->item_type = itDeviceWithNoSignals;

          device->setData(1, q->value("device_name"));
          device->setInfo(1, ItemInfo(itDeviceName, "device_name"));

          p_device_index_list.append(device->index);

        }

        q->finish();
        delete q;

    }

    catch(SvException& e) {

        q->finish();
        delete q;

        throw;
        return false;

    }

    return true;
}

bool MainWindow::pourSignalsToDevices(TreeItem* rootItem)
{
    QSqlError serr;
    QSqlQuery* q = new QSqlQuery(PGDB->db);
    int column_count = rootItem->columnCount();

    try {

        TreeItem *device;

        for(int i = 0; i < rootItem->childCount(); i++) {

          device = rootItem->child(i);

          /* определяем общее кол-во и кол-во привязанных сигналов для этого устройство */
          serr = PGDB->execSQL(QString(SQL_SELECT_SIGNALS_COUNT_STR).arg(device->index), q);
          if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
          q->first();

          // подставляем кол-во сигналов в имя устройства
          device->setData(1, device->data(1).toString() + q->value(0).toString());

          q->finish();

          serr = PGDB->execSQL(QString(SQL_SELECT_INVOLVED_SIGNALS_DEVICE).arg(device->index), q);
          if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

          // заполняем модель выбранными данными
          while(q->next()) {

            TreeItem *signal = device->insertChildren(device->childCount(), 1, column_count);

            signal->index = q->value("signal_index").toInt();
            signal->parent_index = device->index;
            signal->is_main_row = false;
            signal->item_type = q->value("signal_data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog;

            signal->setData(1, q->value("signal_name"));
            signal->setInfo(1, ItemInfo(itSignalName, "signal_name"));

            device->item_type = itDevice;

          }

          q->finish();

        }

        delete q;


  }

  catch(SvException& e) {

    q->finish();
    delete q;

    throw;

    return false;

  }

    return true;

}

bool MainWindow::pourStoragesToRoot(TreeItem *rootItem)
{
    QSqlError serr;
    QSqlQuery* q = new QSqlQuery(PGDB->db);
    int column_count = rootItem->columnCount();

    try {

        serr = PGDB->execSQL(SQL_SELECT_STORAGES_LIST, q);
        if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

        // заполняем модель выбранными данными
        while(q->next()) {

          TreeItem *storage = rootItem->insertChildren(rootItem->childCount(), 1, column_count);

          storage->index = q->value("storage_index").toInt();
          storage->parent_index = rootItem->index;
          storage->is_main_row = true;
          storage->item_type = itStorageWithNoSignals;

          storage->setData(1, q->value("storage_name"));
          storage->setInfo(1, ItemInfo(itStorageName, "storage_name"));

        }

        q->finish();
        delete q;

    }

    catch(SvException& e) {

        q->finish();
        delete q;

        throw;
        return false;

    }

    return true;
}

bool MainWindow::pourDevicesToStorages(TreeItem* rootItem)
{
    QSqlError serr;
    QSqlQuery* q = new QSqlQuery(PGDB->db);
    int column_count = rootItem->columnCount();

    try {

        TreeItem *storage;

        for(int i = 0; i < rootItem->childCount(); i++) {

          storage = rootItem->child(i);

          serr = PGDB->execSQL(QString(SQL_SELECT_DEVICES_BY_STORAGE).arg(storage->index), q);
          if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

          // заполняем модель выбранными данными
          while(q->next()) {

            TreeItem * device = storage->insertChildren(storage->childCount(), 1, column_count);

            device->index = q->value("device_index").toInt();
            device->parent_index = storage->index;
            device->is_main_row = true;
            device->item_type = itDeviceWithNoSignals;

            device->setData(1, q->value("device_name"));
            device->setInfo(1, ItemInfo(itDeviceName, "device_name"));

          }

          q->finish();

        }

        delete q;
        return true;

    }

    catch(SvException& e) {

        q->finish();
        delete q;
        mainlog << sv::log::Time << sv::log::mtCritical << e.error << sv::log::endl;
        throw;
        return false;

    }
}

bool MainWindow::pourSignalsToStorages(TreeItem* rootItem)
{
    QSqlError serr;
    QSqlQuery* q = new QSqlQuery(PGDB->db);
    int column_count = rootItem->columnCount();

    try {

        TreeItem *storage;
        TreeItem *device;

        for(int i = 0; i < rootItem->childCount(); i++) {

          storage = rootItem->child(i);

          for(int j = 0; j < storage->childCount(); j++) {

              device = storage->child(j);

              serr = PGDB->execSQL(QString(SQL_SELECT_SIGNALS_BY_STORAGE_AND_DEVICE).arg(device->index).arg(storage->index), q);
              if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

              // заполняем модель выбранными данными
              while(q->next()) {

                TreeItem* signal = device->insertChildren(device->childCount(), 1, column_count);

                signal->index = q->value("signal_index").toInt();
                signal->parent_index = device->index;
                signal->is_main_row = false;
                signal->item_type = q->value("data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog;

                signal->setData(1, q->value("signal_name"));
                signal->setInfo(1, ItemInfo(itDeviceName, "signal_name"));

                device->item_type = itDevice;
                storage->item_type = itStorage;

              }

              q->finish();

            }
        }

        delete q;
        return true;

    }

    catch(SvException& e) {

        q->finish();
        delete q;
        mainlog << sv::log::Time << sv::log::mtCritical << e.error << sv::log::endl;
        throw e;
        return false;

    }
}

void MainWindow::updateTable()
{
  QList<int> lw;
  for(int i = 0; i < _model->columnCount(); i++)
    lw << ui->treeView->columnWidth(i);

  ui->treeView->setModel(0);

  ui->treeView->setModel(_model);

  for(int i = 0; i < _model->columnCount(); i++)
    ui->treeView->setColumnWidth(i, lw.at(i));

//  ui->treeView->setColumnWidth(7, 200);

//  ui->treeView->resizeColumnToContents();

}

void MainWindow::loadConfig()
{
  if(QMessageBox::question(this, "Предупреждение", "После загрузки конфигурации текущие данные будут утеряны. Продолжить?", QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
      return;

  QString file_name = QFileDialog::getOpenFileName(this, "Имя файла", QString(), "Конфигурации КСУТС (*.ksutscfg);;Все файлы (*.*)");

  if(file_name.isEmpty())
    return;

  /// останавливаем обновление данных
  disconnect(&_update_info_timer, &QTimer::timeout, this, &MainWindow::update_stand_info);
  _update_info_timer.stop();

  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(false);

  ui->textBrowser->setHtml(get_html_page("<p style=\"font-size: 24px\">Загрузка конфигурации.</p>"));

  qApp->processEvents();

  try {

    QFile f(file_name);
    if(!f.open(QFile::ReadOnly))
      _exception.raise(f.errorString());

    QString sql(f.readAll());

    QSqlError serr;
    serr = PGDB->execSQL(sql);
    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    QMessageBox::information(0, "Успешно", QString("Загрузка конфигурации успешно завершена.\n"
                                                      "Необходимо перезапустить программу."));

    qApp->quit();

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", QString("Во время загрузки конфигурации произошла ошибка:\n%1"
                                                  "Необходимо перезапустить программу.")
                          .arg(e.error));

  }
}

void MainWindow::saveConfigAs()
{
  QString file_name = QFileDialog::getSaveFileName(this, "Имя файла", QString(), "Файлы конфигурации КСУТС (*.ksutscfg);;Все файлы (*.*)");

  if(file_name.isEmpty())
    return;

  if(!file_name.endsWith(".ksutscfg"))
    file_name.push_back(".ksutscfg");

  QFile f(file_name);

  bool curr_auth = p_authorized;
  p_authorized = false;
  setAuth();

  try {

    QString cfg = "";
    QString list;

    /// устройства
    list = get_select_list(QString(SQL_SELECT_INVOLVED_DEVICES), "device_index");

    cfg += list.isEmpty() ? "" : QString("update devices set is_configured=true where device_index in (%1);\n").arg(list);
    cfg += QString("update devices set is_configured=false %1;\n")
              .arg(list.isEmpty() ? "" : QString("where device_index not in (%1)").arg(list) );


    /// сигналы
    list = get_select_list(QString(SQL_SELECT_INVOLVED_SIGNALS), "signal_index");

    cfg += list.isEmpty() ? "" : QString("update signals set is_configured=true  where signal_index in (%1);\n").arg(list);
    cfg += QString("update signals set is_configured=false %1;\n")
              .arg(list.isEmpty() ? "" : QString("where signal_index not in (%1)").arg(list) );

    /// привязки сигналов к хранилищам
    /// хранилище 0
    list = get_select_list(QString(SQL_SELECT_FROM_SIGNALS) + " where signals.storage0_linked = true", "signal_index");

    cfg += list.isEmpty() ? "" : QString("update signals set storage0_linked=true  where signal_index in (%1);\n").arg(list);
    cfg += QString("update signals set storage0_linked=false %1;\n")
              .arg(list.isEmpty() ? "" : QString("where signal_index not in (%1)").arg(list) );

    /// хранилище 1
    list = get_select_list(QString(SQL_SELECT_FROM_SIGNALS) + " where signals.storage1_linked = true", "signal_index");

    cfg += list.isEmpty() ? "" : QString("update signals set storage1_linked=true  where signal_index in (%1);\n").arg(list);
    cfg += QString("update signals set storage1_linked=false %1;\n")
              .arg(list.isEmpty() ? "" : QString("where signal_index not in (%1)").arg(list) );

    /// хранилище 2
    list = get_select_list(QString(SQL_SELECT_FROM_SIGNALS) + " where signals.storage2_linked = true", "signal_index");

    cfg += list.isEmpty() ? "" : QString("update signals set storage2_linked=true  where signal_index in (%1);\n").arg(list);
    cfg += QString("update signals set storage2_linked=false %1;\n")
              .arg(list.isEmpty() ? "" : QString("where signal_index not in (%1)").arg(list) );


    if(!f.open(QFile::WriteOnly))
      _exception.raise(f.errorString());

    f.write(cfg.toUtf8());

    f.close();


    QMessageBox::information(this, "Успешно", QString("Резервное копирование успешно завершено"));

  }

  catch (SvException& e) {

    if(f.isOpen())
      f.close();

    QMessageBox::critical(this, "Ошибка", QString("Во время сохранения произошла ошибка:\n%1")
                          .arg(e.error));

  }

  p_authorized = curr_auth;
  setAuth();

}

QString MainWindow::get_select_list(const QString& query, const QString& field)
{
  QString list = "";
  QSqlQuery q(PGDB->db);

  try {

    QSqlError serr = PGDB->execSQL(query, &q);

    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    while(q.next())
      list += QString("%1,").arg(q.value(field).toString());

    q.finish();

    list.chop(1);


  }
  catch(SvException& e) {

    q.finish();
    throw e;

  }

  return list;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About QXmlStream Bookmarks"),
            tr("The <b>QXmlStream Bookmarks</b> example demonstrates how to use Qt's "
               "QXmlStream classes to read and write XML documents."));
}

void MainWindow::createActions()
{

  /** *********** actions ************** **/
  /// редактор списка устройств
  actionEditDeviceList = new QAction(this);
  actionEditDeviceList->setObjectName(QStringLiteral("actionEditDeviceList"));
  actionEditDeviceList->setIcon(QIcon(":/my_icons/icons/002-cpu-1.png"));
  actionEditDeviceList->setText("Конфигурация устройств");
  actionEditDeviceList->setWhatsThis("Редактировать конфигурацию устройств");
  connect(actionEditDeviceList, &QAction::triggered, this, &MainWindow::editDeviceList);

  /// редактор списка сигналов
  actionEditSignalList = new QAction(this);
  actionEditSignalList->setObjectName(QStringLiteral("actionEditSignalList"));
  actionEditSignalList->setIcon(QIcon(":/my_icons/icons/003-graph.png"));
  actionEditSignalList->setText("Конфигурация сигналов");
  actionEditSignalList->setWhatsThis("Редактировать конфигурацию сигналов");
  connect(actionEditSignalList, &QAction::triggered, this, &MainWindow::editSignalList);

  /// хранилища
  actionEditStorages = new QAction(this);
  actionEditStorages->setObjectName(QStringLiteral("actionEditStorages"));
  actionEditStorages->setIcon(QIcon(":/my_icons/icons/016-database.png"));
  actionEditStorages->setText("Привязка сигналов к хранилищам");
  actionEditStorages->setWhatsThis("Редактировать привязку сигналов к хранилищам");
  connect(actionEditStorages, &QAction::triggered, this, &MainWindow::editStorages);

  /// авторизация
  actionAuth = new QAction(tr("&Авторизация"), this);
  actionAuth->setObjectName(QStringLiteral("actionAuth"));
  actionAuth->setIcon(QIcon(":/my_icons/icons/004-lock.png"));
  actionAuth->setWhatsThis("Авторизация");
  actionAuth->setShortcuts(QKeySequence::Undo);
  connect(actionAuth, &QAction::triggered, this, &MainWindow::auth);

  /// редактор конфигурации сервера ксутс
  actionEditConfig = new QAction(tr("Редактор конфигурации"), this);
  actionEditConfig->setObjectName(QStringLiteral("actionEditConfig"));
  actionEditConfig->setIcon(QIcon(":/my_icons/icons/008-content.png"));
  actionEditConfig->setWhatsThis("Редактор конфигурации");
  connect(actionEditConfig, &QAction::triggered, this, &MainWindow::editConfig);

  /// редактор автозапуска
  actionEditAutorun = new QAction(tr("Редактор автозапуска"), this);
  actionEditAutorun->setObjectName(QStringLiteral("actionEditAutorun"));
  actionEditAutorun->setIcon(QIcon(":/my_icons/icons/034-finish-flag.png"));
  actionEditAutorun->setWhatsThis("Редактор автозапуска");
  connect(actionEditAutorun, &QAction::triggered, this, &MainWindow::editAutorun);

  /// создать резервную копию текущей базы данных
  actionBackupDatabase = new QAction(tr("Создать резервную копию БД"), this);
  actionBackupDatabase->setObjectName(QStringLiteral("actionBackupDatabase"));
  actionBackupDatabase->setIcon(QIcon(":/my_icons/icons/037-storing.png"));
  actionBackupDatabase->setWhatsThis("Создать резервную копию БД");
  connect(actionBackupDatabase, &QAction::triggered, this, &MainWindow::backupDB);

  /// восстановить базу данных из резервной копии
  actionRestoreDatabase = new QAction(tr("Восстановить БД"), this);
  actionRestoreDatabase->setObjectName(QStringLiteral("actionRestoreDatabase"));
  actionRestoreDatabase->setIcon(QIcon(":/my_icons/icons/041-folder-3.png"));
  actionRestoreDatabase->setWhatsThis("Восстановить БД");
  connect(actionRestoreDatabase, &QAction::triggered, this, &MainWindow::restoreDB);

  /// загрузить конфигурацию
  actionLoadConfig = new QAction(tr("&Загрузить конфигурацию"), this);
  actionLoadConfig->setObjectName(QStringLiteral("actionLoadConfig"));
  actionLoadConfig->setIcon(QIcon(":/my_icons/icons/011-folder.png"));
  actionLoadConfig->setWhatsThis("Обновить БД");
  actionLoadConfig->setShortcuts(QKeySequence::Open);
  connect(actionLoadConfig, &QAction::triggered, this, &MainWindow::loadConfig);

  /// сохранить текущую конфигурацию
  actionSaveConfig = new QAction(tr("&Сохранить конфигурацию как..."), this);
  actionSaveConfig->setObjectName(QStringLiteral("actionSaveConfig"));
  actionSaveConfig->setIcon(QIcon(":/my_icons/icons/035-diskette-2.png"));
  actionSaveConfig->setWhatsThis("Обновить БД");
  actionSaveConfig->setShortcuts(QKeySequence::SaveAs);
  connect(actionSaveConfig, &QAction::triggered, this, &MainWindow::saveConfigAs);

  /// служебные
  actionExit = new QAction(tr("Выход"), this);
  actionExit->setObjectName(QStringLiteral("actionExit"));
//  actionExit->setIcon(QIcon(":/my_icons/icons/035-diskette-2.png"));
  actionExit->setWhatsThis("Закрыть программу");
  actionExit->setShortcuts(QKeySequence::Quit);
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  actionRestartKsuts = new QAction(tr("Перезапустить КСУТС Сервер"), this);
  connect(actionRestartKsuts, SIGNAL(triggered()), this, SLOT(restartKsutsServer()));

  actionRestartAgg = new QAction(tr("Перезапустить Aggregate Сервер"), this);
  connect(actionRestartAgg, SIGNAL(triggered()), this, SLOT(restartAggServer()));

  restoreAction = new QAction(tr("&Развернуть"), this);
  connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));

  quitAction = new QAction(tr("&Закрыть"), this);
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

}

void MainWindow::createTrayIcon()
{
    p_trayIconMenu = new QMenu(this);
    p_trayIconMenu->addAction(actionRestartKsuts);
    p_trayIconMenu->addAction(actionRestartAgg);
//    p_trayIconMenu->addAction(restoreAction);
    p_trayIconMenu->addSeparator();
    p_trayIconMenu->addAction(quitAction);

    p_trayIcon = new QSystemTrayIcon(this);
    p_trayIcon->setContextMenu(p_trayIconMenu);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {

        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:

            show();
            break;

        case QSystemTrayIcon::MiddleClick:
            showMessage();
            break;

//        case QSystemTrayIcon::Context:
//            showMessage();
//            break;

        default:
            ;
    }
}

void MainWindow::showMessage()
{
//    showIconCheckBox->setChecked(true);
//    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
//            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());

    p_trayIcon->showMessage("Состояние", QString("КСУТС Сервер:\t%1\n"
                                               "AggreGate Сервер:\t%2\n"
                                               "Интерфейс %3:\t%4")
                          .arg("Запущен")
                          .arg("Запущен")
                          .arg("eth0")
                          .arg("Активен"));
}

void MainWindow::messageClicked()
{
    QMessageBox::information(0, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void MainWindow::editDeviceList()
{

  DEVICELIST_UI = new SvDeviceList(this);

  int result = DEVICELIST_UI->exec();

  delete DEVICELIST_UI;

  makeTree();
//  updateTable();

}

void MainWindow::editSignalList()
{
  int device_index = -1;

  switch (_model->itemFromIndex(ui->treeView->currentIndex())->item_type) {

    case itDevice:
    case itDeviceWithNoSignals:
      device_index = _model->itemFromIndex(ui->treeView->currentIndex())->index;
      break;

    case itSignalTypeAnalog:
    case itSignalTypeDiscrete:
      device_index = _model->itemFromIndex(ui->treeView->currentIndex())->parent_index;
      break;

    default:
      return;
  }

  SIGNALLIST_UI = new SvSignalList(device_index, this);

  int result = SIGNALLIST_UI->exec();

  delete SIGNALLIST_UI;

  makeTree();
//  updateTable();

}

void MainWindow::editStorages()
{
  STORAGELIST_UI = new SvStorageList(this);

  int result = STORAGELIST_UI->exec();

  delete STORAGELIST_UI;

  makeTree();
//  updateTable();

}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    TreeItem* item = _model->itemFromIndex(index);

    QList<ItemTypes> types_for_signals({itDevice, itDeviceWithNoSignals, itSignals,itSignal,itSignalTypeAnalog, itSignalTypeDiscrete});
    actionEditSignalList->setEnabled(p_authorized && types_for_signals.contains(item->item_type));

    switch (item->item_type) {

    case itStandRoot:
      ui->textBrowser->setHtml(get_html_page(p_current_stand_info_html));
      break;

    case itAutostart:
      ui->textBrowser->setHtml(get_html_page(get_autostart_info()));
      break;

    case itDevicesRoot:
      ui->textBrowser->setHtml(get_html_page(get_all_devices_info()));
      break;

    case itDevice:
    case itDeviceWithNoSignals:
      ui->textBrowser->setHtml(get_html_page(get_one_device_info(item->index)));
      break;

    case itConfig:
      ui->textBrowser->setHtml(get_html_page(get_server_config()));
      break;

    case itStoragesRoot:
      ui->textBrowser->setHtml(get_html_page(get_all_storages_info()));
      break;

    case itStorage:
    case itStorageWithNoSignals:
      ui->textBrowser->setHtml(get_html_page(get_one_storage_info(item->index)));
      break;

    default:
        ui->textBrowser->setHtml("");
        break;
    }
}

QString MainWindow::get_html_page(const QString& data)
{

  QString template_file_name = QString("%1/info_page.template").arg(_config.templates_dir);

  QFile f(template_file_name);

  try {

    if(!f.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка открытия файла: %1: %2").arg(QFileInfo(f).absoluteFilePath()).arg(f.errorString()));

    QString line(f.readAll());

    f.close();

    return line.replace("%DATA%", data);

  }

  catch(SvException& e) {

    f.close();

    return QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                   "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
                   "p, li { white-space: pre-wrap; }"
                   "</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">"
                   "<p>%1</p>"
                   "</body></html>").arg(e.error);

  }



}

QString MainWindow::get_one_device_info(int index)
{
  QSqlError serr;
  QSqlQuery* q = new QSqlQuery(PGDB->db);

  QString template_file_name = QString("%1/one_device_info.template").arg(_config.templates_dir);

  QFile f(template_file_name);

  try {

    if(!f.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка файла: %1: %2").arg(QFileInfo(f).absoluteFilePath()).arg(f.errorString()));

    QString line(f.readAll());

    f.close();

    serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(index), q);

    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    q->first();

    line.replace("%DEVICE_NAME%", q->value("device_name").toString());
    line.replace("%DEVICE_INDEX%", q->value("device_index").toString());
    line.replace("%DEVICE_PARAMS%", q->value("device_params").toString().replace("\n", "<br>"));
    line.replace("%DEVICE_IFC%", q->value("device_ifc_name").toString());
    line.replace("%DEVICE_IFC_PARAMS%", q->value("device_ifc_params").toString().replace("\n", "<br>"));
    line.replace("%DEVICE_DRIVER%", q->value("device_driver_lib_name").toString());
    line.replace("%DEVICE_DEBUG%", q->value("device_debug").toString());
    line.replace("%DEVICE_DESCRIPTION%", q->value("device_description").toString());
//                line.replace("%DEVICE_", q->value("device_").toString());

    q->finish();
    delete q;

    return line;

  }

  catch(SvException& e) {

    q->finish();
    delete q;

    f.close();

    return QString("<p>%1</p>").arg(e.error);

  }
}

QString MainWindow::get_all_devices_info()
{
  QString result = "";

  for(int i = 0; i < _devicesRoot->childCount(); ++i) {

    result += get_one_device_info(_devicesRoot->child(i)->index);

  }

  return result;
}

void MainWindow::make_stand_info()
{
  QSqlError serr;
  QSqlQuery* q = new QSqlQuery(PGDB->db);

  QString template_file_name = QString("%1/stand_info.template").arg(_config.templates_dir);

  QFile f(template_file_name);

  try {

    if(!f.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка открытия файла: %1").arg(f.errorString()));

    p_current_stand_info_html = QString(f.readAll());

    f.close();


    /** Информация о пульте **/
    serr = PGDB->execSQL(QString(SQL_SELECT_DB_INFO), q);

    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    q->first();

    QString stand_info_str = "<p style=\"font-size: 13px;\">";
    stand_info_str += QString("<strong>Имя пульта:&nbsp;</strong>%1<br>\n").arg(q->value("stand_name").toString());
    stand_info_str += QString("<strong>Версия БД:&nbsp;</strong>%1").arg(q->value("db_version").toString());
    stand_info_str += "</p>";

    p_current_stand_info_hint = QString("Версия БД: %1\n").arg(q->value("db_version").toString());

    q->finish();


    /** сетевые интерфейсы **/
    QStringList ifclist = Netmon::getInterfaceList();
    QString ifc_str = "";
    for(QString ifc: ifclist) {

      ifc_str += "<tr>";

      ifc_str += QString("<td style=\"font-size: 13px; width: 25%;\">&nbsp;%1</td>").arg(ifc);

      IfcStateInfo ifc_info = Netmon::getInterfaceState(ifc);
      ifc_str += QString("<td style=\"font-size: 13px; width: 25%; color: %1\">&nbsp;%2</td>")
          .arg(ifc_info.colors.first().name(QColor::HexRgb))
          .arg(ifc_info.states.first());

      ifc_str += QString("<td style=\"font-size: 13px; width: 25%; color: %1\">&nbsp;%2</td>")
          .arg(ifc_info.colors.last().name(QColor::HexRgb))
          .arg(ifc_info.states.last());

      QString ifc_ip = Netmon::getInterfaceAddr(ifc);
      ifc_str += QString("<td style=\"font-size: 13px; width: 25%;\">&nbsp;%1</td>").arg(ifc_ip);

      ifc_str += "</tr>\n";

      p_current_stand_info_hint += QString("%1: %2\n").arg(ifc).arg(ifc_ip);

    }

    /** сервисы **/
    QString services_str = "";

    // ксутс сервер
    KsutsStateInfo ksuts_state = _ksuts_monitor->getKsutsServerInfo();

    QString ksuts_info_str = "<tr>\n";
    ksuts_info_str += QString("<td style=\"font-size: 13px;\" width=\"40%\">&nbsp;КСУТС Сервер</td>\n");
    ksuts_info_str += QString("<td style=\"font-size: 13px;\" width=\"20%\">&nbsp;%1</td>\n").arg(ksuts_state.version);
    ksuts_info_str += QString("<td style=\"font-size: 13px; color: %1\" width=\"40%\">&nbsp;%2</td>\n")
        .arg(ksuts_state.color.name(QColor::HexRgb))
        .arg(ksuts_state.state);
    ksuts_info_str += "</tr>\n";

    p_current_stand_info_hint += QString("КСУТС Сервер: v.%1: %2\n").arg(ksuts_state.version).arg(ksuts_state.state);

    services_str += ksuts_info_str;

    // СОЭЖ сервер
    SoegStateInfo soeg_state = Soegmon::getSoegServerInfo();

    QString soeg_info_str = "<tr>\n";
    soeg_info_str += QString("<td style=\"font-size: 13px;\" width=\"40%\">&nbsp;СОЭЖ Сервер</td>\n");
    soeg_info_str += QString("<td style=\"font-size: 13px;\" width=\"20%\">&nbsp;%1</td>\n").arg(soeg_state.version);
    soeg_info_str += QString("<td style=\"font-size: 13px; color: %1\" width=\"40%\">&nbsp;%2</td>\n")
        .arg(soeg_state.color.name(QColor::HexRgb))
        .arg(soeg_state.state);
    soeg_info_str += "</tr>\n";

    p_current_stand_info_hint += QString("СОЭЖ Сервер: v.%1: %2\n").arg(soeg_state.version).arg(soeg_state.state);

    services_str += soeg_info_str;

    // aggregate сервер
    AggStateInfo agg_state = Aggmon::getServerInfo();

    QString agg_info_str = "<tr>\n";
    agg_info_str += QString("<td style=\"font-size: 13px;\" width=\"40%\">&nbsp;Aggreagte Сервер</td>\n");
    agg_info_str += QString("<td style=\"font-size: 13px;\" width=\"20%\">&nbsp;%1</td>\n").arg(agg_state.version);
    agg_info_str += QString("<td style=\"font-size: 13px; color: %1\" width=\"20%\">&nbsp;%2</td>\n")
        .arg(agg_state.color.name(QColor::HexRgb))
        .arg(agg_state.state);
    agg_info_str += "</tr>\n";

    p_current_stand_info_hint += QString("Aggreagte Сервер: v.%1: %2\n").arg(agg_state.version).arg(agg_state.state);

    services_str += agg_info_str;


    p_current_stand_info_html.replace("%STAND_INFO%", stand_info_str);
    p_current_stand_info_html.replace("%INTERFACES%", ifc_str);
    p_current_stand_info_html.replace("%SERVICES%", services_str);

    p_trayIcon->setToolTip(p_current_stand_info_hint);
//    mainlog <<  << sv::log::endl;

    delete q;

//    return line;

  }

  catch(SvException& e) {

    q->finish();
    delete q;

    f.close();

    p_current_stand_info_html = QString("<p>%1</p>").arg(e.error);

  }
}

QString MainWindow::get_server_config()
{
  QString template_file_name = QString("%1/ksuts_config_info.template").arg(_config.templates_dir);
  QFile ftmp(template_file_name);

  QString config_file_name = "ksuts_server.cfg";
  QFile fcfg(config_file_name);

  try {

    if(!ftmp.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка открытия файла: %1").arg(ftmp.errorString()));

    if(!fcfg.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка открытия файла: %1").arg(fcfg.errorString()));

    QString line(ftmp.readAll());
    ftmp.close();

    QString data(fcfg.readAll());
    fcfg.close();

    return line.replace("%DATA%", data.replace("\n", "<br>"));

  }

  catch(SvException& e) {

    ftmp.close();
    fcfg.close();

    return QString("<p>%1</p>").arg(e.error);

  }
}

QString MainWindow::get_autostart_info()
{
  QString template_file_name = QString("%1/autostart_info.template").arg(_config.templates_dir);
  QFile ftmp(template_file_name);

  QString ini_file_name = "ksuts_autorun.cfg";
//  QFile fini(ini_file_name);

  try {

    if(!ftmp.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка открытия файла: %1: %2").arg(QFileInfo(ftmp).absoluteFilePath()).arg(ftmp.errorString()));

//    if(!fini.open(QFile::ReadOnly))
//      _exception.raise(QString("Ошибка открытия файла: %1").arg(fini.errorString()));

    QString line(ftmp.readAll());
    ftmp.close();

    /// список команд для выполнения ВНАЧАЛЕ
    int i = 0;
    QString before_str = "";
    while(1) {

        QString cmd = AppParams::readParam(this, "General", QString(COMMAND_BEFORE_n).arg(i++), "", ini_file_name).toString();
        if(cmd.isEmpty())
            break;

        before_str += cmd + "<br>\n";

    }

    /// ждать ли postgres и aggreagte
    QString wait_start_str = "<tr>";
    wait_start_str += AppParams::readParam(this, "General", PARAM_WAIT_POSTGRES, "true", ini_file_name).toBool()
        ? "<td style=\"font-size: 13px;\" width=\"100%\"><strong>&bull;&nbsp;PostgreSQL</strong></td>"
        : "<td style=\"font-size: 13px;color: #707070;\" width=\"100%\">&nbsp;&nbsp;PostgreSQL</td>";

    wait_start_str += "</tr><tr>";
    wait_start_str += AppParams::readParam(this, "General", PARAM_WAIT_AGGREGATE, "true", ini_file_name).toBool()
        ? "<td style=\"font-size: 13px;\" width=\"100%\"><strong>&bull;&nbsp;Aggreagte Server</strong></td>"
        : "<td style=\"font-size: 13px;color: #707070;\" width=\"100%\">&nbsp;&nbsp;Aggreagte Server</td>";
//          "<strong>&bull;&nbsp;Aggreagte Server</strong><br>" : "&nbsp;&nbsp;Aggreagte Server<br>";
    wait_start_str += "</tr>";

    /// список команд для выполнения ПОСЛЕ
    i = 0;
    QString after_str = "";
    while(1) {

        QString cmd = AppParams::readParam(this, "General", QString(COMMAND_AFTER_n).arg(i++), "", ini_file_name).toString();
        if(cmd.isEmpty())
            break;

        after_str += cmd + "<br>\n";

    }

    /// список серверов aggregate
    i = 0;
    QString widget_connection_order_str = "";
    while(1) {

        QString cmd = AppParams::readParam(this, "General", QString(PARAM_WIDGET_HOST_n).arg(i++), "", ini_file_name).toString();
        if(cmd.isEmpty())
            break;

        widget_connection_order_str += cmd + "<br>\n";

    }

    /// параметры для запуска виджета
    QString widget_start_params_str = "";
    widget_start_params_str += QString("user:&nbsp;%1<br>").arg(AppParams::readParam(this, "General", QString(PARAM_WIDGET_USER), "admin", ini_file_name).toString());
    widget_start_params_str += QString("pass:&nbsp;%1<br>").arg(AppParams::readParam(this, "General", QString(PARAM_WIDGET_PASS), "admin", ini_file_name).toString());
    widget_start_params_str += QString("widget:&nbsp;%1<br>").arg(AppParams::readParam(this, "General", QString(PARAM_WIDGET_NAME), "Start_GEU", ini_file_name).toString());
    widget_start_params_str += QString("xms:&nbsp;%1<br>").arg(AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMS), QString("1500"), ini_file_name).toString());
    widget_start_params_str += QString("xmx:&nbsp;%1<br>").arg(AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMX), QString("4500"), ini_file_name).toString());



    return line.replace("%EXEC_BEFORE%", before_str)
               .replace("%WAIT_FOR_START%", wait_start_str)
               .replace("%EXEC_AFTER%", after_str)
               .replace("%WIDGET_START_PARAMS%", widget_start_params_str)
               .replace("%WIDGET_CONNECTION_ORDER%", widget_connection_order_str);

  }

  catch(SvException& e) {

    ftmp.close();

    return QString("<p>%1</p>").arg(e.error);

  }
}

QString MainWindow::get_one_storage_info(int index)
{
  QSqlError serr;
  QSqlQuery* q = new QSqlQuery(PGDB->db);

  QString template_file_name = QString("%1/one_storage_info.template").arg(_config.templates_dir);

  QFile f(template_file_name);

  try {

    if(!f.open(QFile::ReadOnly))
      _exception.raise(QString("Ошибка файла: %1: %2").arg(QFileInfo(f).absoluteFilePath()).arg(f.errorString()));

    QString line(f.readAll());

    f.close();

    serr = PGDB->execSQL(QString(SQL_SELECT_ONE_STORAGE).arg(index), q);

    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    q->first();

    line.replace("%STORAGE_NAME%", q->value("storage_name").toString());
    line.replace("%STORAGE_INDEX%", q->value("storage_index").toString());
    line.replace("%STORAGE_HOST%", q->value("storage_host").toString());
    line.replace("%STORAGE_PORT%", q->value("storage_port").toString());
    line.replace("%STORAGE_DBNAME%", q->value("storage_dbname").toString());
//    line.replace("%STORAGE_DESCRIPTION%", q->value("device_description").toString());

    q->finish();
    delete q;

    return line;

  }

  catch(SvException& e) {

    q->finish();
    delete q;

    f.close();

    return QString("<p>%1</p>").arg(e.error);

  }
}

QString MainWindow::get_all_storages_info()
{
  QString result = "";

  for(int i = 0; i < _storagesRoot->childCount(); ++i) {

    result += get_one_storage_info(_storagesRoot->child(i)->index);

  }

  return result;
}

void MainWindow::update_stand_info()
{
  QString old_stand_info_hint = p_current_stand_info_hint;

  make_stand_info();

  if(_standRoot == _model->itemFromIndex(ui->treeView->currentIndex()) &&
     old_stand_info_hint.compare(p_current_stand_info_hint)) {

//    mainlog << "browser\n"<< ui->textBrowser->toHtml() << "\n\n\nstr\n" << p_current_stand_info_html << sv::log::endl;
    ui->textBrowser->setHtml(p_current_stand_info_html);

  }

}

void MainWindow::restartKsutsServer()
{
  _ksuts_monitor->restartServer();
}

void MainWindow::restartAggServer()
{
  Aggmon::restartServer();
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
  TreeItem* item = _model->itemFromIndex(index);

  switch (item->item_type) {

  case itAutostart:
  {
    if(!p_authorized) break;

    editAutorun();
    ui->textBrowser->setHtml(get_html_page(get_autostart_info()));

    break;
  }

  case itDevicesRoot:
    if(!p_authorized) break;

    editDeviceList();
    makeTree();

    ui->textBrowser->setHtml(get_html_page(get_all_devices_info()));

    break;

  case itDevice:
  case itDeviceWithNoSignals:
  {
    if(!p_authorized) break;

    if(_ksuts_monitor->getKsutsServerInfo().is_active)
    {
      QString n = QString(_config.log_options.log_sender_name).arg(item->index);
//      QString cmd = ;

//      mainlog << "log_sender_name" << mainlog.options().log_sender_name << sv::log::endl;
//      QProcess::startDetached("sudo /home/user/ksuts_server/device_logger -dbus_sender device_40");

      QProcess p;

      if(!p.startDetached(QString("./device_logger -dbus_sender %1").arg(n)))
        mainlog << sv::log::mtError << p.errorString() << sv::log::endl;

    }
    else
    {
      DEVICE_UI = new SvDeviceEditor(this, item->index);

      int result = DEVICE_UI->exec();

      switch (result) {

        case SvDeviceEditor::Error:
        {
          QMessageBox::critical(this, "Ошибка", DEVICE_UI->lastError(), QMessageBox::Ok);
          break;
        }
      }

      delete DEVICE_UI;

    }

    ui->textBrowser->setHtml(get_html_page(get_one_device_info(item->index)));

    break;

  }

  case itStorage:
  case itStorageWithNoSignals:
  {
    if(!p_authorized) break;

    STORAGEEDITOR_UI = new SvStorageEditor(this, item->index);

    int result = STORAGEEDITOR_UI->exec();

    switch (result) {

      case SvStorageEditor::Error:
      {
        QMessageBox::critical(this, "Ошибка", STORAGEEDITOR_UI->lastError(), QMessageBox::Ok);
        break;
      }
    }

    delete STORAGEEDITOR_UI;

    ui->textBrowser->setHtml(get_html_page(get_one_storage_info(item->index)));

    break;
  }


  case itConfig:

    if(!p_authorized) break;
    editConfig();
    ui->textBrowser->setHtml(get_html_page(get_server_config()));
    break;

  default:
      ui->textBrowser->setHtml("");
      break;
  }
}

void MainWindow::auth()
{
  p_authorized = !p_authorized;
  setAuth();
}

void MainWindow::setAuth()
{
  actionAuth->setIcon(p_authorized ? QIcon(":/my_icons/icons/005-lock-1.png") : QIcon(":/my_icons/icons/004-lock.png"));
  actionEditAutorun->setEnabled(p_authorized);
  actionEditConfig->setEnabled(p_authorized);
  actionEditDeviceList->setEnabled(p_authorized);
  actionEditSignalList->setEnabled(p_authorized);
  actionEditStorages->setEnabled(p_authorized);
  actionLoadConfig->setEnabled(p_authorized);
  actionSaveConfig->setEnabled(p_authorized);
  actionBackupDatabase->setEnabled(p_authorized);
  actionRestoreDatabase->setEnabled(p_authorized);
  actionRestartKsuts->setEnabled(p_authorized);
  actionRestartAgg->setEnabled(p_authorized);

  ui->widgetControls->setVisible(p_authorized);
}

void MainWindow::editConfig()
{
  // берем параметры конфигурации сервера!
  QString cfg_file_name = QString("%1%2%3.cfg")
          .arg(QCoreApplication::applicationDirPath())
          .arg(QDir::separator())
          .arg("ksuts_server");

  EDITCONFIG_UI = new SvEditConfig(cfg_file_name, this);

  EDITCONFIG_UI->exec();

  delete EDITCONFIG_UI;

}

void MainWindow::editAutorun()
{
  EDIT_AUTORUN_UI = new EditAutorunWindow(_config.autorun_cfg_file, this);

  int result = EDIT_AUTORUN_UI->exec();

  delete EDIT_AUTORUN_UI;

}

void MainWindow::backupDB()
{
  QString file_name = QFileDialog::getSaveFileName(this, "Имя файла", QString(), "Резервные копии БД КСУТС (*.ksutsdb);;Все файлы (*.*)");

  if(file_name.isEmpty())
    return;

  if(!file_name.endsWith(".ksutsdb"))
    file_name.push_back(".ksutsdb");

  bool curr_auth = p_authorized;
  p_authorized = false;
  setAuth();

  QString pg_bin_path = _config.postgres_bin_path;
  if(pg_bin_path.endsWith(QDir::separator()))
    pg_bin_path.chop(1);

  QProcess p(this);
  int code = p.execute(QString("%1/pg_dump --host %2 --port %3 "
                  "--username \"%4\" --no-password  --format custom "
                  "--blobs --encoding UTF8 --verbose --file \"%5\" \"%6\"")
          .arg(pg_bin_path).arg(_config.db_host)
          .arg(_config.db_port).arg(_config.db_user)
          .arg(file_name).arg(_config.db_name));

  if(code) {

    QMessageBox::critical(this, "Ошибка", QString("Во время резервного копирования произошла ошибка:\n%1")
                          .arg(p.errorString()));

  }
  else {

    QMessageBox::information(this, "Успешно", QString("Резервное копирование успешно завершено"));
//                             .arg(QString(p.errorString())));
  }

  p_authorized = curr_auth;
  setAuth();

}

void MainWindow::restoreDB()
{
  if(QMessageBox::question(this, "Предупреждение", "После восстановления из резервной копии текущие данные будут утеряны. Продолжить?", QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
      return;

  QString file_name = QFileDialog::getOpenFileName(this, "Имя файла", QString(), "Резервные копии БД КСУТС (*.ksutsdb);;Все файлы (*.*)");

  if(file_name.isEmpty())
    return;

  QString pg_bin_path = _config.postgres_bin_path;
  if(pg_bin_path.endsWith(QDir::separator()))
    pg_bin_path.chop(1);

  /// отключаемся от БД, останавливаем обновление данных
  disconnect(&_update_info_timer, &QTimer::timeout, this, &MainWindow::update_stand_info);
  _update_info_timer.stop();

  if(PGDB) {

    delete PGDB;
    PGDB = nullptr;

    QSqlDatabase::removeDatabase(p_connection_name);
  }

  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(false);

  ui->textBrowser->setHtml(get_html_page("<p style=\"font-size: 24px\">Восстановление из резервной копии.</p>"));

  qApp->processEvents();



//  SvBusyWindow *bw = 0;

  try {

//    bw = new SvBusyWindow("Восстановление данных. Подождите.");
//    bw->show();

    QProcess p(this);

    int code = p.execute(QString("%1/dropdb --host %2 --port %3 --username %4 --no-password --echo --if-exists %5")
                          .arg(pg_bin_path).arg(_config.db_host)
                          .arg(_config.db_port).arg(_config.db_user)
                          .arg(_config.db_name));
    if(code)
      _exception.raise(code, p.errorString());

    code  = p.execute(QString("%1/createdb --host %2 --port %3 --username %4 --no-password --echo %5")
                          .arg(pg_bin_path).arg(_config.db_host)
                          .arg(_config.db_port).arg(_config.db_user)
                          .arg(_config.db_name));

    if(code)
      _exception.raise(code, p.errorString());

    code = p.execute(QString("%1/pg_restore --host %2 --port %3 "
                    "--username \"%4\" --no-password  --dbname %5 "
                    "--verbose \"%6\"")
            .arg(pg_bin_path).arg(_config.db_host)
            .arg(_config.db_port).arg(_config.db_user)
            .arg(_config.db_name).arg(file_name));

    if(code)
      _exception.raise(code, p.errorString());


    QMessageBox::information(0, "Успешно", QString("Восстановление из резервной копии успешно завершено.\n"
                                                      "Необходимо перезапустить программу."));

    qApp->quit();

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", QString("Во время восстановления данных произошла ошибка:\n%1")
                          .arg(e.error));

  }
}

void MainWindow::on_bnEditSender_clicked()
{
  ui->lineSender->setEnabled(!ui->lineSender->isEnabled());

  if(!ui->lineSender->isEnabled())
  {
    ui->bnEditSender->setIcon(QIcon(":/my_icons/icons/004-lock.png"));
    return;

  }
  else
  {
    ui->bnEditSender->setIcon(QIcon(":/my_icons/icons/005-lock-1.png"));

  }


  if(ui->lineSender->text().isEmpty() || ui->lineSender->text() == "*")
  {
    QDBusConnection::sessionBus().disconnect(QString(), QString(), DBUS_SERVER_NAME, "message", this, SLOT(currentSenderMessageSlot(const QString&,const QString&,const QString&)));
    QDBusConnection::sessionBus().connect(QString(), QString(), DBUS_SERVER_NAME, "message", this, SLOT(allMessagesSlot(const QString&,const QString&,const QString&)));
  }
  else
  {
    QDBusConnection::sessionBus().disconnect(QString(), QString(), DBUS_SERVER_NAME, "message", this, SLOT(allMessagesSlot(const QString&,const QString&,const QString&)));
    QDBusConnection::sessionBus().connect(QString(), QString(), DBUS_SERVER_NAME, "message", this, SLOT(currentSenderMessageSlot(const QString&,const QString&,const QString&)));
  }

}

void MainWindow::on_bnClear_clicked()
{
    ui->textLog->clear();
}

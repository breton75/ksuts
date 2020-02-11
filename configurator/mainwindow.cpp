#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvPGDB* PGDB;
//extern SvSQLITE* SQLITE;
extern SvStorageList* STORAGELIST_UI;

extern SvSignalList* SIGNALLIST_UI;
extern SvDeviceList* DEVICELIST_UI;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  _monitor = new Ksutsmon(ui->centralWidget);
  ui->vlayMain->addWidget(_monitor);
  _monitor->setVisible(true);

  _net_monitor = new Netmon(ui->centralWidget);
  ui->vlayMain->addWidget(_net_monitor);
  _net_monitor->setVisible(true);

  _agg_monitor = new Aggmon(ui->centralWidget);
  ui->vlayMain->addWidget(_agg_monitor);
  _agg_monitor->setVisible(true);

  setWindowTitle(QString("Конфигуратор пульта v.%1").arg(APP_VERSION));
  setWindowIcon(QIcon(":/signals/icons/application-default-icon_37713.ico"));

  log.setTextEdit(ui->textLog);

  QString s = AppParams::loadLayout(this);

//  ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
  ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
//  ui->treeView->setEditTriggers(QTreeView::AllEditTriggers);
  ui->treeView->setAlternatingRowColors(false);
  ui->treeView->setWordWrap(true);
  createActions();

  fileMenu = menuBar()->addMenu(tr("&Файл"));
  fileMenu->addAction(actionOpen);
  fileMenu->addSeparator();
  fileMenu->addAction(actionSaveAs);
  fileMenu->addSeparator();
  fileMenu->addAction(actionExit);
  fileMenu->addSeparator();

  configMenu = menuBar()->addMenu(tr("&Операции"));
  configMenu->addAction(actionEditDeviceList);
  configMenu->addSeparator();
  configMenu->addAction(actionEditSignalList);
  configMenu->addSeparator();
  configMenu->addAction(actionEditStorages);
  configMenu->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Помощь"));
  helpMenu->addAction(aboutQtAct);


  ui->mainToolBar->addActions(QList<QAction*>() << actionEditDeviceList);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditSignalList);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionEditStorages);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionServerState);

//  helpMenu = menuBar()->addMenu(tr("&Help"));
//  helpMenu->addAction(aboutAct);
//  helpMenu->addAction(aboutQtAct);


//  QMetaObject::connectSlotsByName(this);


#ifdef APP_DEBUG
  _config_path = QString("D:/c++/NIIRPI/configurator/config.db");
#else
  _config_path = QString("%1/config.db").arg(qApp->applicationDirPath());
#endif


}

MainWindow::~MainWindow()
{
  QString s = AppParams::saveLayout(this);

  delete ui;
}

bool MainWindow::init()
{

  QSqlQuery* q = new QSqlQuery(PGDB->db);

  try {

    QSqlError serr = PGDB->execSQL(SQL_SELECT_STORAGES_LIST, q);

    QString header_captions = "";
    if(serr.type() != QSqlError::NoError) {

      QMessageBox::critical(this, "Ошибка", serr.text(), QMessageBox::Ok);
      header_captions = QString(MAIN_TREE_HEADERS).arg("Хр.0").arg("Хр.2").arg("Хр.2");

    }
    else {

      QStringList hl = QStringList();
      while(q->next())
        hl << q->value("storage_name").toString();

      if(hl.count() < 3)
        _exception.raise("В списке хранилищ меньше трех записей");

      header_captions = QString(MAIN_TREE_HEADERS).arg(hl.at(0)).arg(hl.at(1)).arg(hl.at(2));

    }
    q->finish();
    delete q;

    _model = new TreeModel(QString(header_captions).split(';'), ui->treeView);
    ui->treeView->setModel(_model);

    readConfig();
    updateTable();

    ui->treeView->setWordWrap(true);
    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->setUpdatesEnabled(true);

    ui->treeView->setColumnWidth(0, 550);
    ui->treeView->setColumnWidth(1, 90);
    ui->treeView->setColumnWidth(2, 90);
    ui->treeView->setColumnWidth(3, 200);
    ui->treeView->setColumnWidth(4, 90);
    ui->treeView->setColumnWidth(5, 90);
    ui->treeView->setColumnWidth(6, 90);

    return true;

  }

  catch(SvException& e) {
    delete q;
    log << svlog::Critical << e.error << svlog::endl;
    return false;
  }
}

bool MainWindow::readConfig()
{

  QSqlQuery* q = new QSqlQuery(PGDB->db);

  try {

    _model->clear();

    /** заполняем список устройств **/
    // rootItem создается при создании объекта модели
    // кол-во столбцов задается TREE_HEADERS и должно соответствовать кол-ву полей в таблице sensors
    TreeItem *root = _model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();

    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());


    QSqlError serr = PGDB->execSQL(SQL_SELECT_CONFIGURED_DEVICES, q);

    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

    // заполняем модель выбранными данными
    while(q->next()) {

      child_count = root->childCount();

      root->insertChildren(child_count, 1, column_count);

      root->child(child_count)->id = q->value("device_index").toInt();
      root->child(child_count)->parent_id = root->id;
      root->child(child_count)->is_main_row = true; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
//        root->child(child_count)->item_state = q->value("last_state").toInt();
      root->child(child_count)->item_type = itDevice;


      root->child(child_count)->setData(0, q->value("device_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itDeviceName, "device_name"));

      root->child(child_count)->setData(1, QVariant(q->value("device_index")));
      root->child(child_count)->setInfo(1, ItemInfo(itDeviceIndex, "device_index"));

      root->child(child_count)->setData(2, QVariant(q->value("device_ifc_name")));
      root->child(child_count)->setInfo(2, ItemInfo(itDeviceIfcName, "device_ifc_name"));

      root->child(child_count)->setData(3, q->value("device_connection_params").toString().replace(' ', '\n'));
      root->child(child_count)->setInfo(3, ItemInfo(itDeviceParams, "device_connection_params"));

      root->child(child_count)->setData(4, QVariant());
      root->child(child_count)->setInfo(4, ItemInfo());

      root->child(child_count)->setData(5, QVariant());
      root->child(child_count)->setInfo(5, ItemInfo());

      root->child(child_count)->setData(6, QVariant());
      root->child(child_count)->setInfo(6, ItemInfo());

      root->child(child_count)->setData(7, q->value("device_description").toString());
      root->child(child_count)->setInfo(7, ItemInfo(itDeviceDescription, "device_description"));

    }
    q->finish();


    /** заполняем список сигналов **/
    TreeItem *device;

    for(int i = 0; i < root->childCount(); i++) {

      device = root->child(i);

      serr = PGDB->execSQL(QString(SQL_SELECT_CONFIGURED_SIGNALS_DEVICE).arg(device->id), q);

      if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

      // заполняем модель выбранными данными
      while(q->next()) {

        child_count = device->childCount();

        device->insertChildren(child_count, 1, column_count);

        device->child(child_count)->id = q->value("signal_index").toInt();
        device->child(child_count)->parent_id = device->id;
        device->child(child_count)->is_main_row = false; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
  //        root->child(child_count)->item_state = q->value("last_state").toInt();
        device->child(child_count)->item_type = q->value("signal_data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog;

        device->child(child_count)->setData(0, q->value("signal_name"));
        device->child(child_count)->setInfo(0, ItemInfo(itSignalName, "signal_name"));

        device->child(child_count)->setData(1, q->value("signal_index"));
        device->child(child_count)->setInfo(1, ItemInfo(itSignalCobId, "signal_index"));

        device->child(child_count)->setData(2, QVariant());
        device->child(child_count)->setInfo(2, ItemInfo());

        device->child(child_count)->setData(3, QVariant());
        device->child(child_count)->setInfo(3, ItemInfo());

        device->child(child_count)->setData(4, QVariant(q->value("signal_storage0_linked")));
        device->child(child_count)->setInfo(4, ItemInfo(itSignalStorageLink, "signal_storage0_linked"));

        device->child(child_count)->setData(5, QVariant(q->value("signal_storage1_linked")));
        device->child(child_count)->setInfo(5, ItemInfo(itSignalStorageLink, "signal_storage1_linked"));

        device->child(child_count)->setData(6, QVariant(q->value("signal_storage2_linked")));
        device->child(child_count)->setInfo(6, ItemInfo(itSignalStorageLink, "signal_storage2_linked"));

        device->child(child_count)->setData(7, q->value("signal_description"));
        device->child(child_count)->setInfo(7, ItemInfo(itSignalDescription, "signal_description"));

      }

      q->finish();

    }

    delete q;

    return true;

  }

  catch(SvException& e) {

    delete q;
    log << svlog::Time << svlog::Critical << e.error << svlog::endl;
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

void MainWindow::saveAs()
{
//    QString fileName =
//            QFileDialog::getSaveFileName(this, tr("Save Bookmark File"),
//                                         QDir::currentPath(),
//                                         tr("XBEL Files (*.xbel *.xml)"));
//    if (fileName.isEmpty())
//        return;

//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

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
  QIcon icon1;
  icon1.addFile(QStringLiteral(":/signals/icons/1systemerroralt_103270.ico"), QSize(), QIcon::Normal, QIcon::Off);
//  icon1.addFile(QStringLiteral(":/signals/icons/107cpu_99914.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditDeviceList = new QAction(this);
  actionEditDeviceList->setObjectName(QStringLiteral("actionEditDeviceList"));
  actionEditDeviceList->setIcon(icon1);
  actionEditDeviceList->setText("Конфигурация устройств");
  actionEditDeviceList->setWhatsThis("Редактировать конфигурацию устройств");
  connect(actionEditDeviceList, &QAction::triggered, this, &MainWindow::editDeviceList);

  QIcon icon2;
  icon2.addFile(QStringLiteral(":/signals/icons/2virtualbox_103102.ico"), QSize(), QIcon::Normal, QIcon::Off);
//  icon2.addFile(QStringLiteral(":/signals/icons/146soundwaves_99890.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditSignalList = new QAction(this);
  actionEditSignalList->setObjectName(QStringLiteral("actionEditSignalList"));
  actionEditSignalList->setIcon(icon2);
  actionEditSignalList->setText("Конфигурация сигналов");
  actionEditSignalList->setWhatsThis("Редактировать конфигурацию сигналов");
  connect(actionEditSignalList, &QAction::triggered, this, &MainWindow::editSignalList);

  /// хранилища
  QIcon icon3;
  icon3.addFile(QStringLiteral(":/signals/icons/3systemconfigsamba_103331.ico"), QSize(), QIcon::Normal, QIcon::Off);
//  icon3.addFile(QStringLiteral(":/signals/icons/015upload1_99960.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditStorages = new QAction(this);
  actionEditStorages->setObjectName(QStringLiteral("actionEditStorages"));
  actionEditStorages->setIcon(icon3);
  actionEditStorages->setText("Привязка сигналов к хранилищам");
  actionEditStorages->setWhatsThis("Редактировать привязку сигналов к хранилищам");
  connect(actionEditStorages, &QAction::triggered, this, &MainWindow::editStorages);

  /// состояние сервера
  QIcon icon4;
  icon4.addFile(QStringLiteral(":/signals/icons/4terminal_103094.ico"), QSize(), QIcon::Normal, QIcon::Off);
//  icon4.addFile(QStringLiteral(":/signals/icons/maintenance.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionServerState = new QAction(this);
  actionServerState->setObjectName(QStringLiteral("actionServerState"));
  actionServerState->setIcon(icon4);
  actionServerState->setText("Состояние сервера");
  actionServerState->setWhatsThis("Состояние сервера");
  connect(actionServerState, &QAction::triggered, this, &MainWindow::serverState);

  /// служебные
  actionOpen = new QAction(tr("&Открыть конфигурацию"), this);
  actionOpen->setShortcuts(QKeySequence::Open);
//  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  actionSaveAs = new QAction(tr("&Сохранить конфигурацию как..."), this);
  actionSaveAs->setShortcuts(QKeySequence::SaveAs);
  connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

  actionExit = new QAction(tr("E&xit"), this);
  actionExit->setShortcuts(QKeySequence::Quit);
  connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


}

void MainWindow::editDeviceList()
{

  DEVICELIST_UI = new SvDeviceList(this);

  int result = DEVICELIST_UI->exec();

  delete DEVICELIST_UI;

  readConfig();
  updateTable();

}

void MainWindow::editSignalList()
{
  int device_index = -1;

  switch (_model->itemFromIndex(ui->treeView->currentIndex())->item_type) {
    case itDevice:
      device_index = _model->itemFromIndex(ui->treeView->currentIndex())->id;
      break;

    case itSignalTypeAnalog:
    case itSignalTypeDiscrete:
      device_index = _model->itemFromIndex(ui->treeView->currentIndex())->parent_id;
      break;

    default:
      return;
  }

  SIGNALLIST_UI = new SvSignalList(device_index, _model->itemFromIndex(ui->treeView->currentIndex())->data(0).toString(), this);

  int result = SIGNALLIST_UI->exec();

  delete SIGNALLIST_UI;

  readConfig();
  updateTable();

}


void MainWindow::editStorages()
{
  STORAGELIST_UI = new SvStorageList(this);

  int result = STORAGELIST_UI->exec();

  delete STORAGELIST_UI;

  readConfig();
  updateTable();

}

void MainWindow::serverState()
{
#ifdef Q_WIN
    QProcess::startDetached("dbupdate.exe"); //  * p = new QProcess(this);
#else
  bool b = QProcess::startDetached(QString("%1/server_monitor").arg(qApp->applicationDirPath()));

  log << "server_monitor:" << QString("%1").arg(b) << svlog::endl;

#endif
}

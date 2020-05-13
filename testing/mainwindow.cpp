#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_bnTestJson_clicked()
{
    QJsonDocument j = QJsonDocument::fromJson(QString("{\"RS485\":{\"portname\":\"ttyS8\",\"databits\":8}}").toUtf8());

    QJsonValue v;


    QJsonObject jo_main;

    QJsonObject j485 = j["RS485"].toObject();
    j485["databits"] = QJsonValue(int(7));


    QJsonObject judp = j["UDP"].toObject();
    judp["host"] = QJsonValue("127.0.0.1");

    jo_main.insert("RS485", j485);
    jo_main.insert("UDP", judp);



    j.setObject(jo_main);

//    jo.["portname"] = "jkjk";

//    j. setObject(jo); // = jo.toVariantMap();
    qDebug() /*<< jo.value("portname") << jo.value("databits") */<< j.toJson(QJsonDocument::Compact);
}


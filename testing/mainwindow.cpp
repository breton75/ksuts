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


void MainWindow::on_pushButton_clicked()
{
    QByteArray ba = QByteArray::fromHex(ui->lineCmd->text().toUtf8());

    QUdpSocket u5800;

    u5800.bind(5800);

    QUdpSocket u5301;
    u5301.writeDatagram(ba, QHostAddress("192.168.1.50"), 5301);
    u5301.close();

    qDebug() << u5301.errorString();
//u.bind(5300);
    qDebug() << 1;
    while(u5800.waitForReadyRead(1000))
    {
        qDebug() << 2;
        while(u5800.hasPendingDatagrams())
        {
            qDebug() << 3;

            void* datagram = malloc(0xFFFF);
            quint64 g = u5800.readDatagram((char*)datagram, 0xFFFF);

            QByteArray b((const char*)datagram, g);

            qDebug() << b.toHex();
        }
    }
    qDebug() << u5800.errorString();
    u5800.close();
}

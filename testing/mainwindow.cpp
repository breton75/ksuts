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

    QUdpSocket u;


    u.bind(quint16(ui->spinPort->value()));
        u.writeDatagram(ba, QHostAddress("192.168.1.50"), quint16(ui->spinPort->value()));

//void* datagram = malloc(0xFFFF);

//        while (true) {

//          while(u.waitForReadyRead(1000))
//          {
//            while(u.hasPendingDatagrams())
//            {
//              qint64 datagram_size = u.pendingDatagramSize();

//              if(datagram_size <= 0 || datagram_size > 0xFFFF) continue;

//              u.readDatagram((char*)(datagram), datagram_size);

//              QByteArray ab((char*)datagram, datagram_size);

//              ui->lineResponse->setText(QString(ab.toHex()));
//            qDebug() << "got data";


//            }
//          }
//          qApp->processEvents();
//        }

//free(datagram);
    qDebug() << "done";

}

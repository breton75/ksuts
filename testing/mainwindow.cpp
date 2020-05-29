#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->lineIP->setText(AppParams::readParam(this, "General", "Host", "").toString());
  ui->spinRecvPort->setValue(AppParams::readParam(this, "General", "RecvPort", 5300).toUInt());
  ui->spinTransmitPort->setValue(AppParams::readParam(this, "General", "TransmitPort", 5300).toUInt());
  ui->checkRecv->setChecked(AppParams::readParam(this, "General", "Recv", false).toBool());

}


MainWindow::~MainWindow()
{

  AppParams::saveParam(this, "General", "Host", ui->lineIP->text());
  AppParams::saveParam(this, "General", "RecvPort", ui->spinRecvPort->value());
  AppParams::saveParam(this, "General", "TransmitPort", ui->spinTransmitPort->value());
  AppParams::saveParam(this, "General", "Recv", ui->checkRecv->isChecked());

  delete ui;
}


void MainWindow::on_bnTestJson_clicked()
{
//    QJsonDocument j = QJsonDocument::fromJson(QString("{\"RS485\":{\"portname\":\"ttyS8\",\"databits\":8}}").toUtf8());

//    QJsonValue v;


//    QJsonObject jo_main;

//    QJsonObject j485 = j["RS485"].toObject();
//    j485["databits"] = QJsonValue(int(7));


//    QJsonObject judp = j["UDP"].toObject();
//    judp["host"] = QJsonValue("127.0.0.1");

//    jo_main.insert("RS485", j485);
//    jo_main.insert("UDP", judp);



//    j.setObject(jo_main);

////    jo.["portname"] = "jkjk";

////    j. setObject(jo); // = jo.toVariantMap();
//    qDebug() /*<< jo.value("portname") << jo.value("databits") */<< j.toJson(QJsonDocument::Compact);
}


void MainWindow::on_pushButton_clicked()
{
    QByteArray ba = QByteArray::fromHex(ui->lineCmd->text().toUtf8());

    QUdpSocket uRecv;

    if(ui->checkRecv->isChecked())
      uRecv.bind(quint16(ui->spinRecvPort->value()));

    QUdpSocket uTrnsmt;
    uTrnsmt.writeDatagram(ba, QHostAddress(ui->lineIP->text()), quint16(ui->spinTransmitPort->value()));
    uTrnsmt.close();

    qDebug() << uTrnsmt.errorString();

    if(ui->checkRecv->isChecked())
    {
      while(uRecv.waitForReadyRead(4000))
      {
          qDebug() << 2;
          while(uRecv.hasPendingDatagrams())
          {
              qDebug() << 3;

              void* datagram = malloc(0xFFFF);
              quint64 g = uRecv.readDatagram((char*)datagram, 0xFFFF);

              QByteArray b((const char*)datagram, g);

              qDebug() << b.toHex();
          }
      }

      qDebug() << uRecv.errorString();
      uRecv.close();

    }
}

void MainWindow::on_bnRecvOnly_clicked()
{
  QUdpSocket uRecv;

  uRecv.bind(quint16(ui->spinRecvPort->value()));
qDebug() << ui->spinRecvPort->value();

  for(int i = 0; i < 5; ++i) {
  while(uRecv.waitForReadyRead(1000))
  {
//      qDebug() << 2;
      while(uRecv.hasPendingDatagrams())
      {
//          qDebug() << 3;

          void* datagram = malloc(0xFFFF);
          quint64 g = uRecv.readDatagram((char*)datagram, 0xFFFF);

          QByteArray b((const char*)datagram, g);
//          for(int i = 0; i < b.length(); i++)
//            b[i] = ~b.at(i);

          ui->textRecv->append(QString(b.toHex()));
          qApp->processEvents();

      }
  }
}
  qDebug() << uRecv.errorString();
  uRecv.close();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

quint32 stringToSeconds(const QString& str, bool* ok = nullptr);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
//  _client = new svtcp::SvTcpClient(ui->editIp->text(), ui->spinPort->value(), this);
  _log.setTextEdit(ui->textLog);

  QString a = "12h1m1s";

  bool b;
  quint32 i = stringToSeconds(a, &b);

    qDebug() << i << b;

}

MainWindow::~MainWindow()
{
  delete ui;
}

QTextEdit* MainWindow::get_log()
{
  return ui->textLog;
}

quint32 stringToSeconds(const QString& str, bool* ok)
{
  bool b = true;
  quint64 result;

  result = str.toUInt(&b);
  if(b) return result; // если значение сразу задано в секундах

  int hpos = 0, mpos = 0, spos = 0;
  QString h = "0", m = "0", s = "0";

  if(str.contains("h")) {

    hpos = str.indexOf("h");
    h = str.mid(0, hpos);
    hpos++;
    mpos = hpos;
  }


  if(str.contains("m")) {

    mpos = str.indexOf("m");
    m = str.mid(hpos, mpos - hpos);
    mpos++;
  }

  if(str.contains("s")) {

    spos = str.indexOf("s");
    s = str.mid(mpos, spos - mpos);
  }

  quint32 H = h.toUInt(&b);
  quint32 M = m.toUInt(&b);
  quint32 S = s.toUInt(&b);

  if(!b) {

      if(ok) *ok = false;
      return 0;
  }
  else {

      if(ok) *ok = true;
      return H * 3600 + M * 60 + S;
  }

}

void MainWindow::on_bnConnect_clicked()
{
  if(!_client) {

    _client = new svtcp::SvTcpClient(ui->editIp->text(), ui->spinPort->value(), ui->textLog);
    _client->setLog(_log);
    _client->setFlags(svtcp::LogInData | svtcp::InDataAsHex | svtcp::LogOutData | svtcp::OutDataAsHex);

    if(_client->connectToHost() != svtcp::SOCKET_OK) {

      ui->textLog->append(_client->lastError());
      return;

    }

    ui->bnConnect->setStyleSheet("background-color: rgb(245, 40, 40);");
    ui->bnConnect->setText("Disconnect");

  }
  else {

    _t.stop();
    disconnect(&_t, SIGNAL(timeout()), this, SLOT(send()));

    _client->disconnectFromHost();
    delete _client;
    _client = nullptr;

    ui->bnConnect->setStyleSheet("background-color: rgb(85, 255, 127);");
    ui->bnConnect->setText("Connect");

  }

}

void MainWindow::on_bnSendOnce_clicked()
{
  send();
}

void MainWindow::send()
{

  if(!_client) return;

  quint16 start_register = quint16(ui->spinStartReg->value());
  quint16 register_count = quint16(ui->spinRegCount->value());

  qsrand(QTime::currentTime().msecsSinceStartOfDay());
  if(ui->checkRndStartReg->isChecked())
    start_register = qrand() % 269; // всего 270 регистров, начиная с 0

  if(ui->checkRndRegCount->isChecked())
    register_count = qrand() % 124 + 1; // максимум можно запросить 125 регистров

  /** форимруем запрос **/
  QByteArray b = QByteArray::fromHex("00 00 00 00 06 00 FF 04 00 00 00 00");

  b[0] = quint8(_transaction_cnt >> 8);
  b[1] = quint8(_transaction_cnt & 0x00FF);

  b[8] = quint8(start_register >> 8);
  b[9] = quint8(start_register & 0x00FF);

  b[10] = quint8(register_count >> 8);
  b[11] = quint8(register_count & 0x00FF);

  /** отправляем **/
  _client->sendData(b);

  _transaction_cnt++;

}

void MainWindow::on_bnSendCycle_clicked()
{
  connect(&_t, SIGNAL(timeout()), this, SLOT(send()));
  _t.start(1000);
}

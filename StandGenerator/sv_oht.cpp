#include "sv_oht.h"

SvOHT::SvOHT():
  SvAbstractSystem(),
  p_widget(new QWidget), 
  ui(new Ui::MainWidget)  
{
  ui->setupUi(p_widget);
  
  p_state.state = RunState::FINISHED;
  p_state.mode = EditMode::SAVED; 
  
  ui->listDirections_0x13->clear();
  ui->cbRegim_0x13->clear();
  ui->cbState_0x13->clear();
  ui->cbURSState_0x13->clear();  
  
  for(int i = 0; i < StatesCodes.count(); ++i)
    ui->cbState_0x13->addItem(StatesCodes.at(i).first);
  
  for(int i = 0; i < RegimCodes.count(); ++i)
    ui->cbRegim_0x13->addItem(RegimCodes.at(i).first);
  
  for(int i = 0; i < URSStatesCodes.count(); ++i)
    ui->cbURSState_0x13->addItem(URSStatesCodes.at(i).first);
      
  
  for(int i = 0; i < DirectionsCodes.count(); ++i) {
    
    ui->listDirections_0x13->addItem(DirectionsCodes.at(i).first);
    ui->listDirections_0x13->item(i)->setCheckState(Qt::Unchecked);
    
    p_data.data_0x13.append(Type_0x13(DirectionsCodes.at(i).second,
                                      StatesCodes.at(0).second,
                                      RegimCodes.at(0).second,
                                      URSStatesCodes.at(0).second,
                                      0));
    
  }
  
  p_widget->setVisible(true);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvOHT::startStopPressed);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvOHT::on_bnEditData_clicked);
  
}

SvOHT::~SvOHT()
{
  delete ui;
  delete p_widget;
  
  deleteLater();
}

void SvOHT::startStopPressed()
{
  switch (p_state.state) {
    
    case RunState::RUNNING:

      setState(RunState::STOPPING);
      p_thread->stop();
      
      break;
      
    case RunState::FINISHED:
    {
      setState(RunState::STARTING);
      
      SerialParamsParser p(ui->editPortParams->text());
      if(!p.parse()) {
        
        qDebug() << p.lastError();
        
        setState(RunState::FINISHED);
        
        return;
        
      }
      
      p_port_params = p.serialParams();
      
      p_thread = new SvOHTThread(&p_port_params, ui->spinInterval->value(), &p_edit_mutex, &p_data);
      connect(p_thread, &SvOHTThread::finished, this, &SvOHT::threadFinished);
//      connect(p_thread, &SvOHTThread::finished, p_thread, &QThread::deleteLater);
      
      try {
        
        p_thread->open();
        p_thread->start();
        
        
      } catch(SvException& e) {
        
        qDebug() << e.error;
        
        threadFinished();
        
        return;
        
      }
      
      setState(RunState::RUNNING);
      
      break;
      
    }
      
    default:
      break;
  }

}

void SvOHT::threadFinished() 
{
  if(p_thread) {
    
    delete p_thread;
    p_thread = nullptr;
    
  }
  
  setState(RunState::FINISHED);
  
}

void SvOHT::setState(RunState state)
{
  foreach (QWidget* w, p_widget->findChildren<QWidget*>())
    w->setEnabled(false);

  
  switch (state) {
    
    case RunState::STARTING:
      
      p_state.state = RunState::STARTING;
      break;
      
    case RunState::STOPPING:
      
      p_state.state = RunState::STOPPING;
      break;
      
    case RunState::RUNNING:
      
      foreach (QWidget* w, p_widget->findChildren<QWidget*>())
        w->setEnabled(true);
      
      ui->editPortParams->setEnabled(false);
      ui->bnPortParams->setEnabled(false);
      
      p_state.state = RunState::RUNNING;
      
      setMode(EditMode::SAVED);
      
      ui->bnStartStop->setText("Стоп");
      ui->bnStartStop->setStyleSheet("background-color: tomato");
                
      break;
      
      
    case RunState::FINISHED:
      
      foreach (QWidget* w, p_widget->findChildren<QWidget*>())
        w->setEnabled(true);
      
      p_state.state = RunState::FINISHED;
      p_state.mode = EditMode::SAVED;
      
      ui->bnStartStop->setText("Старт");
      ui->bnStartStop->setStyleSheet("background-color: palegreen;");
      
      break;
      
    default:
      break;
  }
  
  qApp->processEvents();
  
}

void SvOHT::setMode(EditMode mode)
{
  p_state.mode = mode;
  
  switch (mode) {
    
    case EditMode::EDITING:
      
      ui->bnEditData->setText("Принять");
      
      foreach (QWidget* w, ui->group0x13->findChildren<QWidget*>())
        w->setEnabled(true);
      
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      foreach (QWidget* w, ui->group0x13->findChildren<QWidget*>())
        w->setEnabled(false);
      
      break;      
      
    default:
      break;
  }
  
  ui->bnEditData->setEnabled(true);
  
}

void SvOHT::on_bnEditData_clicked()
{
  ui->bnEditData->setEnabled(false);
  qApp->processEvents();
  
  switch (p_state.mode) {
    
    case EditMode::EDITING:      
      
      setData();
      
      setMode(EditMode::SAVED);
      
      break;
    
    case EditMode::SAVED:
      
      setMode(EditMode::EDITING);
      
      break;
      
      
    default:
      break;
  }
}

bool SvOHT::setData()
{
  bool result = p_edit_mutex.tryLock(3000);
//  qDebug() << "lock" << result;
  if(result) {

    for(int i = 0; i < p_data.data_0x13.count(); ++i) {
      
      if(ui->listDirections_0x13->item(i)->checkState() == Qt::Checked) {
        
        p_data.data_0x13[i].set(StatesCodes.at(ui->cbState_0x13->currentIndex()).second,
                                   RegimCodes.at(ui->cbRegim_0x13->currentIndex()).second,
                                   URSStatesCodes.at(ui->cbURSState_0x13->currentIndex()).second,
                                   quint8(ui->checkOtklVent_0x13->isChecked() ? 0x01 : 0x00));
        
      } else {
      
        p_data.data_0x13[i].set(0, 0, 0, 0);
      
      }
    
    
    }
    
//    qDebug() << "unlock";
    p_edit_mutex.unlock();
    
  }
  
  return result;
  
}

/**         SvOHTThread         **/
SvOHTThread::SvOHTThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OHTData *data):
  p_port_params(params),
  p_timeout(timeout),
  is_active(false)
{
  p_mutex = mutex;
  p_data = data;
}

void SvOHTThread::open() throw(SvException&)
{
  p_port.setPortName(p_port_params->portname);
  p_port.setBaudRate(p_port_params->baudrate);
  p_port.setStopBits(p_port_params->stopbits);
  p_port.setFlowControl(p_port_params->flowcontrol);
  p_port.setDataBits(p_port_params->databits);
  p_port.setParity(p_port_params->parity);
  
  if(!p_port.open(QIODevice::ReadWrite))
    throw exception.assign(p_port.errorString());
    
}

void SvOHTThread::run() 
{
   is_active = true;
   
   while(is_active) {
     
     if(p_mutex->tryLock(3000)) {
       
       QByteArray b = QByteArray::fromHex(QString("01100AA00022441342"
                                                  "010000000000"
                                                  "020000000000"
                                                  "030000000000"
                                                  "040000000000"
                                                  "050000000000"
                                                  "060000000000"
                                                  "070000000000"
                                                  "080000000000"
                                                  "090000000000"
                                                  "0A0000000000"
                                                  "0B0000000000"
                                                  "0000").toUtf8());
       
       for(int i = 0; i < DirectionsCodes.count(); ++i) {
         
         b[10 + i] = p_data->data_0x13.at(i).byte1.toUint8();
//         b[9 + i] |= p_data->data_0x13.at(i).byte1.regim << 8;
         b[11 + i] = p_data->data_0x13.at(i).byte2.toUint8();
//         b[10 + i] |= p_data->data_0x13.at(i).byte2.otkl_vent << 7;
         
       }
       qDebug() << QString(b.toHex().toUpper());
       
       p_mutex->unlock();
       
     }
     
     
     QThread::msleep(p_timeout);
//     qDebug() << SerialParamsParser::getSring(*p_port_params);
   }
   
   emit finished();
}


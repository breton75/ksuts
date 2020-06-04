#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../ksuts_devices/opa/opa_defs.h"
#include "../../svlib/sv_pgdb.h"

#define HEADER "<value name=\"expression\">"
#define TAIL "</value>"

extern SvPGDB* PGDB;

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

void MainWindow::on_bnDec2Hex_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "*.prs");

    if(fnl.isEmpty()) return;
    
    for(QString fn: fnl) {
    
      QFile f(fn);
      if(!f.open(QFile::Text | QFile::ReadOnly)) {
        
        QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
        return;      
      }
      
      qDebug() << fn;
      
      QString amp = "&amp;";
      QString amp1 = QString("%1 ").arg(amp);
      QString amp2 = QString("%1%1 ").arg(amp);
      QStringList new_text;
      
      while(!f.atEnd()) {
        
        QByteArray line = f.readLine();
        
        while(true) {
          
          if(line.contains(amp2.toUtf8())) {
            
            int ampstart = line.indexOf(amp2);
            int numstart = ampstart + amp2.length();
            int numend = numstart;
            
            QString numstr = "";
            while(QChar(line.at(numend)).isDigit())
              numstr.append(line.at(numend++));
            
            
            if(!numstr.trimmed().isEmpty()) {
              
              QString hex = "0x";
              bool a;
              quint64 v = numstr.toULongLong(&a);
              
              if(a) {
                
                hex += QString("%1").arg(v, 16, 16).replace(' ', '0').toUpper();
              
                line.replace(numstart, numend - numstart, hex.toUtf8());
              
//                qDebug() << line;
//                qDebug() << hex;
              }
            }
            
            line.replace(ampstart, amp2.length(), QString("%1%1[~]").arg(amp).toUtf8());
            
          }
          else if(line.contains(amp1.toUtf8())) {
            
            int ampstart = line.indexOf(amp1);
            int numstart = ampstart + amp1.length();
            int numend = numstart;
            
            QString numstr = "";
            while(QChar(line.at(numend)).isDigit())
              numstr.append(line.at(numend++));
            
//            qDebug() << numstr;
            numstr = numstr.trimmed();
    //        qDebug() << numstr;
            if(!numstr.isEmpty()) {
              
              QString hex = "0x";
              bool a;
              quint64 v = numstr.toULongLong(&a);
              
              if(a) {
                
                QString h = QString("%1").arg(v, 16, 16).replace(' ', '0').toUpper();
              
                while(h.left(2) == "00")
                  h = h.right(h.length() - 2);
                
                hex += h;
                  
                line.replace(numstart, numend - numstart, hex.toUtf8());
              
//                qDebug() << line;
//                qDebug() << hex;
              }
            }
            
            line.replace(ampstart, amp1.length(), QString("%1[~]").arg(amp).toUtf8());
            
          }
          else
            break;
          
        }
        
        line.replace(QString("%1[~]").arg(amp), QString("%1 ").arg(amp).toUtf8());
        
        new_text.append(line);
        
      }
      f.close();
        
      
      QFileInfo fi(fn);
      QString newfn = fi.path() + "/_h_" + fi.fileName();
//      newfn.chop(fi.suffix().length() + 1);
//      newfn += "_hexed." + fi.suffix();
      qDebug() << newfn;
      f.setFileName(newfn);
      if(!f.open(QFile::Text | QFile::WriteOnly)) {
        
        QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
        return;      
      }
      
      for(QString line: new_text)
        f.write(line.toUtf8());
      
      f.close();
      
      qDebug() << "done";
    }
}

void MainWindow::on_bnAnalogPattern_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_h_*.prs");

  if(fnl.isEmpty()) return;
  
  QString aPattern("st('val_%1_%2', st('val', catch(cell({users.admin.queries.getAnalogValue:execute(%1, %2)}, \"cdaCan_data_analogDev_data\"), null)) == null ? -3.0 : ld('val'))");
//  QString aPattern("st('val_%1 _%2', st(%1, %2))");
  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString asp = "select({users.admin.devices.cms_db:";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();
      
      while(true) {
        
        if(line.contains(asp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString("</value>").toUtf8())) {
            qDebug() << line;
            line += '\n' + f.readLine();
          }
          
          int asp_start = line.indexOf(asp);
          int canid_start = asp_start + asp.length();
          int canid_end = canid_start;
          QString canid_str = "";
          
          while(QChar(line.at(canid_end)).isDigit())
            canid_str.append(line.at(canid_end++));
          
          
          if(canid_str.trimmed().isEmpty()) {
            
            panic = QString("Panic! NO NUMBER: %1").arg(QString(line));
//            qDebug() << panic;
//            QMessageBox::critical(this, "", panic, QMessageBox::Ok);
            
            break;
            
          }
          
          int devindex_end = line.indexOf(')', canid_end) - 1;
          int devindex_start = devindex_end;
          QString devindex_str = "";
          
          while(QChar(line.at(devindex_start)) != QChar(','))
            devindex_str.push_front(line.at(devindex_start--)); 
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
          
         line.replace(asp_start, devindex_end - asp_start + 2, QString("[~%1;%2~]").arg(canid_str).arg(devindex_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      
      
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString pat(line.mid(r_start, r_end - r_start + 2));
          QStringList sl = QString(line.mid(r_start + 2, r_end - r_start - 2)).split(';');
          
          if(sl.count() != 2) {
            qDebug() << "panic!";
            break;
          }
          
          qDebug() << pat << sl.at(0) << sl.at(1);
          
          line.replace(r_start, r_end - r_start + 2, QString(aPattern).arg(sl.at(0)).arg(sl.at(1)).toUtf8());
          
          line.replace(QString("[~%1;%2~]").arg(sl.at(0)).arg(sl.at(1)).toUtf8(), QString("ld('val_%1_%2')").arg(sl.at(0)).arg(sl.at(1)).toUtf8());
          
//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        else
          break;
        
   
      }
      
      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();
    
    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 3, "_ah_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}
    

void MainWindow::on_bnDigitalPattern_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_ah_*.prs");

  if(fnl.isEmpty()) return;
  
//  QString aPattern("st('val_%1_%2', st('val', catch(cell({users.admin.queries.getAnalogValue:execute(%1, %2)}, \"cdaCan_data_analogDev_data\"), null)) == null ? -3.0 : ld('val'))");
  QString dPattern("st('val_%1', catch(cell({users.admin.queries.getDiscreteValue:execute(%1)}, \"cddCan_data_digitalDev_data\"), null))");
  
  
  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString dsp = "{users.admin.devices.cms_db:";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();
      
      while(true) {
        
        if(line.contains(dsp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString("</value>").toUtf8())) 
            line += f.readLine();
          
          int dsp_start = line.indexOf(dsp);
          int canid_start = dsp_start + dsp.length();
          int canid_end = canid_start;
          QString canid_str = "";
          
          while(QChar(line.at(canid_end)).isDigit())
            canid_str.append(line.at(canid_end++));
          
          
          if(canid_str.trimmed().isEmpty()) {
            
            panic = QString("Panic! NO NUMBER: %1").arg(QString(line));
//            qDebug() << panic;
//            QMessageBox::critical(this, "", panic, QMessageBox::Ok);
            
            break;
            
          }
          
          int dsp_end = line.indexOf('}', canid_end);
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
         
         line.replace(dsp_start, dsp_end - dsp_start + 1, QString("[~%1~]").arg(canid_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      
      
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString canid_str(line.mid(r_start + 2, r_end - r_start - 2));
          
          line.replace(r_start, r_end - r_start + 2, QString(dPattern).arg(canid_str).toUtf8());
          
          line.replace(QString("[~%1~]").arg(canid_str).toUtf8(), QString("ld('val_%1')").arg(canid_str).toUtf8());
          
//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        else
          break;
        
   
      }
      
      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();
    
    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 4, "_dah_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}

void MainWindow::on_bnSignalPattern_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_dah_*.prs");

  if(fnl.isEmpty()) return;

  QString sPattern("st('val_%1', cell({users.admin.queries.getSignalValue:execute(\"%1\")}, \"sdvSignals_data_rsValue\"))");
//  QString aPattern("st('val_%1 _%2', st(%1, %2))");
  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString ssp = "select({users.admin.devices.cms_db:signals_data_rs}";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();
      
      while(true) {
        
        if(line.contains(ssp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString("</value>").toUtf8())) {
            line += '\n' + f.readLine();
          }
          
          int ssp_start = line.indexOf(ssp);
          int ssp_end = line.indexOf(')', ssp_start);
          int sname_start = ssp_end;
          QString sname_str = "";
          
          while(QChar(line.at(sname_start)) != QChar('\''))
            sname_start--;
          
          sname_start--;
          
          while(QChar(line.at(sname_start)) != QChar('\''))
            sname_str.push_front(line.at(sname_start--)); 
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
          
         line.replace(ssp_start, ssp_end - ssp_start + 1, QString("[~%1~]").arg(sname_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      
      
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString sname(line.mid(r_start + 2, r_end - r_start - 2));
          
          if(!SGNS.contains(sname))
            qDebug() << "no signal" << sname;
  
//          line.replace(r_start, r_end - r_start + 2, QString(sPattern).arg(sname).arg(SGNS[sname]).toUtf8());
          line.replace(r_start, r_end - r_start + 2, QString(sPattern).arg(sname).toUtf8());
          line.replace(QString("[~%1~]").arg(sname).toUtf8(), QString("ld('val_%1')").arg(sname).toUtf8());
          
          
//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        else
          break;
        
   
      }
      
      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();
    
    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 5, "_sdah_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}

void MainWindow::on_bnDetectors_clicked()
{
  QString fnd = "D:/c++/KSUTS/документация/detectors.txt";
  QString fns = "D:/c++/KSUTS/документация/signals.csv";
  
//  QString s1 = ";BI22_2SS1_DT11_T1;Сработка 2СС1-ДТ11 температура в Помещении холод. шкафов 30 гр.Ц.\n";
//  QString s2 = "2СС1-ДТ11";
//  qDebug() << s1.contains(s2);
  
//  return;
  
  QStringList signals_list;
  QStringList new_signals_list;
  
  QFile fs(fns);
  if(!fs.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", fs.errorString(), QMessageBox::Ok);
    return;      
  }
  
  while(!fs.atEnd()) {
    signals_list << QString(fs.readLine());
//    qDebug() << signals_list.last();
  }
  
  fs.close();
//  qDebug() << fns;

  QFile fd(fnd);
  if(!fd.open(QFile::Text | QFile::ReadOnly)) {
    
    QMessageBox::critical(this, "", fd.errorString(), QMessageBox::Ok);
    return;      
  }
  
  qDebug() << fnd;
  
  while(!fd.atEnd()) {
    
    QString line_d = QString(fd.readLine());

    QStringList p = line_d.split(";");
    if(p.count() != 2) {
      qDebug() << "wrong line_d" << line_d;
      break;
    }
    
    QString det_indx = p.at(0);
    QString det_name = p.at(1);
//    qDebug() << p.at(0) << det_name;
    
    for(int i = 0; i < signals_list.count(); i++) {
      
      QString line_s = signals_list.at(i);
      qDebug() << line_s << det_name;
      if(line_s.startsWith(';') && line_s.contains(det_name.trimmed())) {
        signals_list[i] = det_indx.trimmed() + line_s; // .push_front(p.at(0));
//        qDebug() << line_s << det_name;
//      new_signals_list << line_s;
      }
      
    }
   
  }
  fd.close();

  
  QFileInfo fi(fns);
  QString newfn = fi.fileName();
  newfn.push_front("_sd_");
  newfn = fi.path() + "/" + newfn;
  
  fs.setFileName(newfn);
  if(!fs.open(QFile::Text | QFile::WriteOnly)) {
    QMessageBox::critical(this, "", fs.errorString(), QMessageBox::Ok);
    return;      
  }
  
  for(QString line: signals_list)
    fs.write(line.toUtf8());
  
  fs.close();
  
  qDebug() << "fininshed";
    
}


void MainWindow::on_bnAnalogPatternWithoutSdLd_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_h_*.prs");

  if(fnl.isEmpty()) return;
  
  QString aPattern("%1st('val_%2_%3', st('val', catch(cell({users.admin.queries.getAnalogValue:execute(%2, %3)}, \"cdaCan_data_analogDev_data\"), null)) == null ? -3.0 : ld('val'))");
  
  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString asp = "select({users.admin.devices.cms_db:";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();
      
      QByteArray commented;
      commented.clear();

      QByteArray header;
      header.clear();
      
      while(true) {
        
        if(line.contains(asp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString(TAIL).toUtf8()))
            line += f.readLine();

          if(line.contains(HEADER)) {

            header = line.left(line.indexOf(HEADER) + QString(HEADER).length());
            line.replace(header, "");
          }

          if(line.indexOf("/*") > -1 && line.indexOf("*/") > line.indexOf("/*") + 2) {

            int beg = line.indexOf("/*");
            int end = line.indexOf("*/");

            commented = line.mid(beg, end - beg + 2) + "\n\n";
            line.replace(commented, "");

//            qDebug() << commented;
//            qDebug() << "";

          }

          line = line.trimmed();
//          qDebug() << "trimmed: " << line;
          
          int asp_start = line.indexOf(asp);
          int canid_start = asp_start + asp.length();
          int canid_end = canid_start;
          QString canid_str = "";
          
          while(QChar(line.at(canid_end)).isDigit())
            canid_str.append(line.at(canid_end++));
          
          
          if(canid_str.trimmed().isEmpty()) {
            
            panic = QString("Panic! NO NUMBER: %1").arg(QString(line));
//            qDebug() << panic;
//            QMessageBox::critical(this, "", panic, QMessageBox::Ok);
            
            break;
            
          }
          
          int devindex_end = line.indexOf(')', canid_end) - 1;
          int devindex_start = devindex_end;
          QString devindex_str = "";
          
          while(QChar(line.at(devindex_start)) != QChar(','))
            devindex_str.push_front(line.at(devindex_start--)); 
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
          
         line.replace(asp_start, devindex_end - asp_start + 2, QString("[~%1;%2~]").arg(canid_str).arg(devindex_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      
      QByteArray signals_sum;
      signals_sum.clear();
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString pat(line.mid(r_start, r_end - r_start + 2));
          QStringList sl = QString(line.mid(r_start + 2, r_end - r_start - 2)).split(';');
          
          if(sl.count() != 2) {
            qDebug() << "panic!";
            break;
          }
          
          qDebug() << pat << sl.at(0) << sl.at(1);
          
          signals_sum += QString(aPattern).arg(!signals_sum.isEmpty() ? " + \n" : "").arg(sl.at(0)).arg(sl.at(1)).toUtf8();
//          line.replace(r_start, r_end - r_start + 2, QString(aPattern).arg(sl.at(0)).arg(sl.at(1)).toUtf8());
          
          line.replace(QString("[~%1;%2~]").arg(sl.at(0)).arg(sl.at(1)).toUtf8(), QString("ld('val_%1_%2')").arg(sl.at(0)).arg(sl.at(1)).toUtf8());
          
          
          
//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        
        else
          break;
        
   
      }

      if(!signals_sum.isEmpty()) {

        signals_sum += QString(" + 1 == 0 ? 'impossible value' : \n").toUtf8();

        line.push_front(signals_sum);

//        line.replace("<value name=\"expression\">", QString("<value name=\"expression\">%1\n%2").arg(QString(commented)).arg(QString(signals_sum)).toUtf8());

//        qDebug() << line;
//        qDebug() << "";
//        qDebug() << "";
      }

      if(!commented.isEmpty())
        line.push_front(commented);

      if(!header.isEmpty())
        line.push_front(header);

//      qDebug() << "finish line: " << line;
      
      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();

    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 3, "_Ah_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}
    
void MainWindow::on_bnDigitalPatternWithoutSdLd_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_Ah_*.prs");

  if(fnl.isEmpty()) return;
  
//  QString aPattern("st('val_%1_%2', st('val', catch(cell({users.admin.queries.getAnalogValue:execute(%1, %2)}, \"cdaCan_data_analogDev_data\"), null)) == null ? -3.0 : ld('val'))");
  QString dPattern("%1st('val_%2', catch(cell({users.admin.queries.getDiscreteValue:execute(%2)}, \"cddCan_data_digitalDev_data\"), null))");
  
  
  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString dsp = "{users.admin.devices.cms_db:";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();

      QByteArray commented;
      commented.clear();
      
      QByteArray header;
      header.clear();

      while(true) {
        
        if(line.contains(dsp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString(TAIL).toUtf8()))
            line += f.readLine();
          
          if(line.contains(HEADER)) {

            header = line.left(line.indexOf(HEADER) + QString(HEADER).length());
            line.replace(header, "");
          }

          if(line.indexOf("/*") > -1 && line.indexOf("*/") > line.indexOf("/*") + 2) {

            int beg = line.indexOf("/*");
            int end = line.indexOf("*/");

            commented = line.mid(beg, end - beg + 2) + "\n\n";
            line.replace(commented, "");

//            qDebug() << commented;
//            qDebug() << "";

          }

          line = line.trimmed();
//          qDebug() << "trimmed: " << line;

          int dsp_start = line.indexOf(dsp);
          int canid_start = dsp_start + dsp.length();
          int canid_end = canid_start;
          QString canid_str = "";
          
          while(QChar(line.at(canid_end)).isDigit())
            canid_str.append(line.at(canid_end++));
          
          
          if(canid_str.trimmed().isEmpty()) {
            
            panic = QString("Panic! NO NUMBER: %1").arg(QString(line));
//            qDebug() << panic;
//            QMessageBox::critical(this, "", panic, QMessageBox::Ok);
            
            break;
            
          }
          
          int dsp_end = line.indexOf('}', canid_end);
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
         
         line.replace(dsp_start, dsp_end - dsp_start + 1, QString("[~%1~]").arg(canid_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      

      QByteArray signals_sum;
      signals_sum.clear();
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString canid_str(line.mid(r_start + 2, r_end - r_start - 2));
          
          signals_sum += QString(dPattern).arg(!signals_sum.isEmpty() ? " + \n" : "").arg(canid_str).toUtf8();
//          line.replace(r_start, r_end - r_start + 2, QString(dPattern).arg(canid_str).toUtf8());
          
          line.replace(QString("[~%1~]").arg(canid_str).toUtf8(), QString("ld('val_%1')").arg(canid_str).toUtf8());
          
//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        else
          break;
        
   
      }
      
      if(!signals_sum.isEmpty()) {

        signals_sum += QString(" + 1 == 0 ? 'impossible value' : \n").toUtf8();

        line.push_front(signals_sum);

//        line.replace("<value name=\"expression\">", QString("<value name=\"expression\">%1\n%2").arg(QString(commented)).arg(QString(signals_sum)).toUtf8());

//        qDebug() << line;
//        qDebug() << "";
//        qDebug() << "";
      }

      if(!commented.isEmpty())
        line.push_front(commented);

      if(!header.isEmpty())
        line.push_front(header);

//      qDebug() << "finish line: " << line;

      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();
    
    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 4, "_DAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}

void MainWindow::on_bnSignalPatternWithoutSdLd_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups/OKC+GAMMA_backup_130918/models/h";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "_DAh_*.prs");

  if(fnl.isEmpty()) return;
  
  QString sPattern("%1st('val_%2', cell({users.admin.queries.getSignalValue:execute(\"%2\")}, \"sdvSignals_data_rsValue\"))");

  for(QString fn: fnl) {
  
    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    qDebug() << fn;
    
    QString ssp = "select({users.admin.devices.cms_db:signals_data_rs}";
    QStringList new_text;
    QString panic = "";
        
    while(!f.atEnd()) {
      
      QByteArray line = f.readLine();

      QByteArray commented;
      commented.clear();

      QByteArray header;
      header.clear();
      
      while(true) {
        
        if(line.contains(ssp.toUtf8())) {
          
          while(!line.trimmed().endsWith(QString(TAIL).toUtf8()))
            line += f.readLine();

          if(line.contains(HEADER)) {

            header = line.left(line.indexOf(HEADER) + QString(HEADER).length());
            line.replace(header, "");
          }

          if(line.indexOf("/*") > -1 && line.indexOf("*/") > line.indexOf("/*") + 2) {

            int beg = line.indexOf("/*");
            int end = line.indexOf("*/");

            commented = line.mid(beg, end - beg + 2) + "\n\n";
            line.replace(commented, "");

//            qDebug() << commented;
//            qDebug() << "";

          }

          line = line.trimmed();
//          qDebug() << "trimmed: " << line;

          int ssp_start = line.indexOf(ssp);
          int ssp_end = line.indexOf(')', ssp_start);
          int sname_start = ssp_end;
          QString sname_str = "";
          
          while(QChar(line.at(sname_start)) != QChar('\''))
            sname_start--;
          
          sname_start--;
          
          while(QChar(line.at(sname_start)) != QChar('\''))
            sname_str.push_front(line.at(sname_start--)); 
            
//         qDebug() << line;
//         qDebug() << canid_str << devindex_str;
          
         line.replace(ssp_start, ssp_end - ssp_start + 1, QString("[~%1~]").arg(sname_str).toUtf8());
         
//         qDebug() << line;
//         qDebug() << line;
          
        }
        else
          break;
        
      }
      
      QByteArray signals_sum;
      signals_sum.clear();
      while(true) {
        
        if(line.contains("[~")) {
          
          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");
          
          QString sname(line.mid(r_start + 2, r_end - r_start - 2));
          
          if(!SGNS.contains(sname))
            qDebug() << "no signal" << sname;
  
          signals_sum += QString(sPattern).arg(!signals_sum.isEmpty() ? " + \n" : "").arg(sname).toUtf8();
//          line.replace(r_start, r_end - r_start + 2, QString(sPattern).arg(sname).arg(SGNS[sname]).toUtf8());
//          line.replace(r_start, r_end - r_start + 2, QString(sPattern).arg(sname).toUtf8());
          line.replace(QString("[~%1~]").arg(sname).toUtf8(), QString("ld('val_%1')").arg(sname).toUtf8());
          

//          qDebug() << line;
//          qDebug() <<  ' ';
          
        }
        else
          break;
        
   
      }
      
      if(!signals_sum.isEmpty()) {

        signals_sum += QString(" + 1 == 0 ? 'impossible value' : \n").toUtf8();

        line.push_front(signals_sum);

//        line.replace("<value name=\"expression\">", QString("<value name=\"expression\">%1\n%2").arg(QString(commented)).arg(QString(signals_sum)).toUtf8());

//        qDebug() << line;
//        qDebug() << "";
//        qDebug() << "";
      }

      if(!commented.isEmpty())
        line.push_front(commented);

      if(!header.isEmpty())
        line.push_front(header);

//      qDebug() << "finish line: " << line;

      new_text.append(line);
      
//      if(!panic.isEmpty()) break;
      
    }
    
    f.close();
    
    QFileInfo fi(fn);
    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 5, "_SDAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {
      
      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;      
    }
    
    for(QString line: new_text)
      f.write(line.toUtf8());
    
    f.close();
    
    qDebug() << "done";
    
//    qDebug() << "done";
    
  }
}

void MainWindow::on_bnPlus1ToDummy_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user/ksuts_backups_new/Models/_ALL";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "*.prs");

  if(fnl.isEmpty()) return;

  QString sPattern(")) == 'dummy value' ? 'dummy value'");

  for(QString fn: fnl) {

    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    qDebug() << fn;

    QString dummysp = ")) + 0xFF == 0 ? color(255, 0, 255)";
    QStringList new_text;

    while(!f.atEnd()) {

      QByteArray line = f.readLine();

      if(line.contains(sPattern.toUtf8())) {

        line.replace(sPattern, dummysp.toUtf8());

      }

      new_text.append(line);

    }

    f.close();

    QFileInfo fi(fn);
    QString newfn = fi.path() + "/new/" + fi.fileName(); //.push_front("_1SDAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    for(QString line: new_text)
      f.write(line.toUtf8());

    f.close();

  }

  qDebug() << "done";

}

void MainWindow::on_bnTreatGAMMA_clicked()
{

  SvPGDB* PGDB = new SvPGDB();

  QString db = "cms_db";
  QString host = "localhost";
  QString user = "postgres";
  QString pass = "postgres";

  PGDB->setConnectionParams(db, host, 5432, user, pass);

  QApplication::processEvents();

  QSqlError serr = PGDB->connectToDB();

  if((serr.type() != QSqlError::NoError)) {

    qDebug() << serr.databaseText();
    return;
  }
  else qDebug() << "connected!";


/** ************************************* **/
//  QString lst = "";
//  foreach (int key, signals_f_Treshold1.keys())
//    lst += QString("'%1',").arg(signals_f_Treshold1.value(key));

//  lst.chop(1);

//  int cnt = 0;
//  QString lst2 = "";
//  QSqlQuery* q = new QSqlQuery(PGDB->db);
//  serr = PGDB->execSQL(QString("select signal_index from signals where signal_name IN (%1)").arg(lst), q);

//  while(q->next()) {

//    lst2 += QString("%1,").arg(q->value("signal_index").toString());

//        if(cnt == 10) {
//          cnt = 0;
//          ui->textEdit->append(lst2);
//          lst2 = "";
//        }
//        else cnt++;
//  }
/** ************************************* **/

    QSqlQuery* q = new QSqlQuery(PGDB->db);
    serr = PGDB->execSQL(QString("SELECT id, signal_name, timeout, timeout_value, "\
                                 "data_offset, data_length, data_type, description, signal_index, "\
                                 "timeout_signal_index "\
                            "FROM signals where device_index = 20"), q);

    if(serr.type() != QSqlError::NoError)
      qDebug() << serr.text();

    QString tmpl = "INSERT INTO signals(device_index, signal_name, timeout, timeout_value, data_offset, "\
                       "data_length, data_type, description, signal_index, timeout_signal_index, is_configured) "\
                       "VALUES (%1, '%2', %3, %4, %5, %6, %7, '%8', %9, %10, false);";
    int cnt = 0;
    while(q->next()) {


//      int device_index = q->value("device_index").toInt();
      QString signal_name = q->value("signal_name").toString();
//      qDebug() << signal_name;

      int timeout = q->value("timeout").toInt();
      int timeout_value = q->value("timeout_value").toInt();
      int data_offset = q->value("data_offset").toInt();
      int data_length = q->value("data_length").toInt();
      int data_type = q->value("data_type").toInt();
      QString description = q->value("description").toString();
      int signal_index = q->value("signal_index").toInt();
      int timeout_signal_index = q->value("timeout_signal_index").toInt();
//      bool is_configured = q->value("is_configured").toBool();
//      bool storage0_linked = q->value("storage0_linked").toBool();
//      bool storage1_linked = q->value("storage1_linked").toBool();
//      bool storage2_linked = q->value("storage2_linked").toBool();


      QString insstr = "";
      if(gamma119.contains(signal_index)) {
        insstr = QString(tmpl).arg(2000).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);

        ui->textEdit->append(insstr);
      }

      if(gamma122.contains(signal_index)) {
        insstr = QString(tmpl).arg(21).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);

        ui->textEdit->append(insstr);
      }

      if(gamma218.contains(signal_index)) {
        insstr = QString(tmpl).arg(22).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);

        ui->textEdit->append(insstr);
      }

      if(gamma123.contains(signal_index)) {
        insstr = QString(tmpl).arg(27).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }

      if(gamma38.contains(signal_index)) {
        insstr = QString(tmpl).arg(24).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }

      if(gamma93.contains(signal_index)) {
        insstr = QString(tmpl).arg(26).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }

      if(gamma8.contains(signal_index)) {
        insstr = QString(tmpl).arg(23).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }

      if(gamma67.contains(signal_index)) {
        insstr = QString(tmpl).arg(25).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }

      if(gamma150.contains(signal_index)) {
        insstr = QString(tmpl).arg(28).arg(signal_name).arg(timeout).arg(timeout_value).arg(data_offset)
            .arg(data_length).arg(data_type).arg(description).arg(signal_index).arg(timeout_signal_index);
        ui->textEdit->append(insstr);
      }
//      qDebug() << insstr;
//      ui->textEdit->insertPlainText(insstr);

      cnt++;

    }

  qDebug() << "done" << cnt;
}


void MainWindow::on_pushButton_2_clicked()
{
#ifdef Q_OS_LINUX
  QString fpath = "/home/user";
#else
  QString fpath = "D:/c++/KSUTS/ksuts_backups/OKC+GAMMA_backup_130918/models";
#endif

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "*.prs");

  if(fnl.isEmpty()) return;

  QString sPattern = "<value name=\"target\">.:";
  QString sPattern2 = "/*###*/";
  QString nPattern("select({users.admin.devices.database2:signals_data_rs}, 'value', 'signal_name', '%1') == 0.0 ? 0 : 1");

  for(QString fn: fnl) {

    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    qDebug() << fn;

    QStringList new_text;
    QString bindsum = "";

    while(!f.atEnd()) {

      QByteArray line = f.readLine();

      if(line.contains(sPattern.toUtf8())) {

        QString sigend = "";
        for(int i = 1; i < 5; i++)
          sigend.push_front(QChar(line.at(line.indexOf("$dev_data") - i)));

        int bindbeg = line.indexOf(sPattern) + sPattern.length() - 2;
        int bindlen = line.indexOf("</value>") - bindbeg;
        QString bindn = QString(line.mid(bindbeg, bindlen));

        new_text.append(line);

        /** **************** **/
        line = f.readLine();

        if(line.contains(sPattern2.toUtf8())) {

            QString sign = "";
            foreach (QString s, SIGNALS_BY_ROOMS.values()) {
              if(s.endsWith(sigend)) {

                sign = s;
                break;

              }
            }

            if(!sign.isEmpty()) {

              line.replace(sPattern2, QString(nPattern).arg((sign)) .toUtf8());
              bindsum += " + {" + bindn + "}";

            }

            else
              qDebug() << "no end " << sigend;



        }
      }

      new_text.append(line);

    }

    f.close();

    ui->textEdit->setText(bindsum);

    QFileInfo fi(fn);
    QString newfn = fi.path() + "/new_" + fi.fileName(); //.push_front("_1SDAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    for(QString line: new_text)
      f.write(line.toUtf8());

    f.close();

  }
}

void MainWindow::on_bnJoinCANSignals_clicked()
{

  /** FILE **/
  QString fn = "/home/user/tmp/Signals_12700_can.csv";

  QFile f(fn);
  if(!f.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  qDebug() << fn;

  /** DATABASE **/
  if(!connectPGDB())
    return;

  QString update_signal_data = "UPDATE signals SET cob_id=%1, data_offset=%2, data_length=%3, data_type=%4 WHERE signal_name = '%5'";


  int index = 1361;

  /** TREATING **/
  while(!f.atEnd()) {

    QByteArray line = f.readLine();

    QStringList p = QString(line).split(';');

    QString signal_name = p.at(0);
    QString signal_desc = p.at(1);
    QString can_id = p.at(2);
    QString signal_offset = p.at(3);
    QString signal_len = p.at(4);
    QString signal_data_type = p.at(5) == "BOOL" ? "0" : "1";

    if(can_id.toUpper().contains("НЕТ") || can_id == "")
      continue;

    qDebug() << signal_name << index;

    QSqlError serr = PGDB->execSQL(QString("INSERT INTO signals(cob_id, device_index, signal_name, timeout, timeout_value, "\
                          "data_offset, data_length, data_type, description, signal_index, "\
                          "timeout_signal_index, is_configured, storage0_linked, storage1_linked, storage2_linked) "\
                  "VALUES (%1, 0, '%2', 3000, %3, "\
                          "%4, %5, %6, '%7', %8, "\
                          "0, false, false, false, false)")
                  .arg(can_id).arg(signal_name).arg(signal_data_type == "0" ? -1 : -3)
                  .arg(signal_offset).arg(signal_len).arg(signal_data_type).arg(signal_desc).arg(index));

    if(serr.type() != QSqlError::NoError) {
      qDebug() << serr.text();
      break;
    }

    serr = PGDB->execSQL(QString("INSERT INTO signals_data(signal_index, last_update, value) "\
                  "VALUES (%1, now(), %2) ")
                  .arg(index).arg(signal_data_type == "0" ? -1 : -3));

    if(serr.type() != QSqlError::NoError) {
      qDebug() << serr.text();
      break;
    }

    index++;

  }
  qDebug() << "done";

}

bool MainWindow::connectPGDB(QString db_name)
{
  PGDB = new SvPGDB();

  QString db = db_name;
  QString host = "localhost";
  QString user = "postgres";
  QString pass = "postgres";

  PGDB->setConnectionParams(db, host, 5432, user, pass);

  QApplication::processEvents();

  QSqlError serr = PGDB->connectToDB();

  if((serr.type() != QSqlError::NoError)) {

    qDebug() << serr.databaseText();
    return false;
  }
  else qDebug() << "connected!";

  return true;
}

void MainWindow::on_bnTreatSKM_clicked()
{

  /** FILE **/
  QString fn = "/home/user/tmp/GAMMA_PD_SKM.prs";

  QString sPattern = "<value name=\"target\">.:";
  QString sPattern2 = "<value name=\"expression\">select";
  QString nPattern( "\t\t\t\t\t\t<value name=\"target\">.:%1$dev_data</value>\n");
  QString nPattern2("\t\t\t\t\t\t<value name=\"expression\">select({users.admin.devices.database2:signals_data_rs}, 'value', 'signal_name', '%1') == 1.0 ? 1 : 0</value>\n");

  QFile f(fn);
  if(!f.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  qDebug() << fn;

  /** DATABASE **/
  if(!connectPGDB())
    return;

  QString select_input_signals = "SELECT signal_name, signal_index FROM signals where device_index=39 order by signal_index ASC";

  QSqlQuery* q = new QSqlQuery(PGDB->db);
  QSqlError serr = PGDB->execSQL(QString(select_input_signals), q);

  if(serr.type() != QSqlError::NoError) {
    qDebug() << serr.text();
    return;
  }


  /** TREATING **/
  int linecount = 0;
  while(linecount++ < 85445)
    f.readLine();

  QStringList new_text;
  QString sName = "";
  QByteArray newline;

  while(!f.atEnd()) {

    QByteArray line = f.readLine();

    if(line.contains(sPattern.toUtf8())) {

      if(!q->next())
        break;

      sName = q->value("signal_name").toString();

      newline = QString(nPattern).arg(sName).toUtf8();

    }
    else if(line.contains(sPattern2.toUtf8())) {

      newline = QString(nPattern2).arg(sName).toUtf8();

    }

    else
      newline = line;


    new_text.append(newline);

  }

  delete q;
  PGDB->disconnectFromDb();
  f.close();

  QFileInfo fi(fn);
  QString newfn = fi.path() + "/new_" + fi.fileName(); //.push_front("_1SDAh_");

  f.setFileName(newfn);
  if(!f.open(QFile::Text | QFile::WriteOnly)) {

    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  for(QString line: new_text)
    f.write(line.toUtf8());

  f.close();


  qDebug() << "done";




}





void MainWindow::on_bnSOEG_1_clicked()
{
  /** FILE **/
  QString fn = "/home/user/tmp/soeg_1_v3_8.csv";

  QFile f(fn);
  if(!f.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  qDebug() << fn;

  /** DATABASE **/
  if(!connectPGDB())
    return;

  QString update_soeg_data = "UPDATE signals SET soeg_register=%1, soeg_offset=%2, soeg_data_length=%3, soeg_data_type='%4' WHERE signal_name = '%5'";

  /** TREATING **/
  while(!f.atEnd()) {

    QByteArray line = f.readLine();

    QStringList p = QString(line).split(',');

    QString sName = p.at(0);
    QString dType = p.at(1);
    int data_len = QString(p.at(2)).toInt();
    int reg_num = QString(p.at(3)).toInt();
    int offset = QString(p.at(4)).toInt();

    if(dType == "unsigned int") dType = "u";
    else if(dType == "real") dType = "r";
    else if(dType == "boolean") dType = "b";
    else if(dType == "word") dType = "w";


    QSqlError serr = PGDB->execSQL(QString(update_soeg_data).arg(reg_num).arg(offset).arg(data_len).arg(dType).arg(sName));

    if(serr.type() != QSqlError::NoError) {
      qDebug() << serr.text();
    }

  }

  f.close();
  PGDB->disconnectFromDb();

  qDebug() << "done";

}

void MainWindow::on_bnTreatCANSignals_clicked()
{
  /** FILE **/
  QString fn = "/home/user/tmp/Signals_12700_can.csv";

  QFile f(fn);
  if(!f.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  qDebug() << fn;

  /** DATABASE **/
  if(!connectPGDB())
    return;

  QString update_signal_data = "UPDATE signals SET cob_id=%1, data_offset=%2, data_length=%3, data_type=%4 WHERE signal_name = '%5'";

  /** TREATING **/
  while(!f.atEnd()) {

    QByteArray line = f.readLine();

    QStringList p = QString(line).split(',');

    QString sName = p.at(0);
    int can_id = QString(p.at(1)).toInt();
    int offset = QString(p.at(2)).toInt();
    int data_len = QString(p.at(3)).toInt();
    int dType = data_len == 1 ? 0 : 1;


    QSqlError serr = PGDB->execSQL(QString(update_signal_data).arg(can_id).arg(offset).arg(data_len).arg(dType).arg(sName));

    if(serr.type() != QSqlError::NoError) {
      qDebug() << serr.text();
    }

  }

  f.close();
  PGDB->disconnectFromDb();

  qDebug() << "done";

}

void MainWindow::on_bnGetLostSignals_clicked()
{
  /** FILE **/
  QString fn = "/home/user/tmp/soeg_1_v3_8.csv";

  QFile f(fn);
  if(!f.open(QFile::Text | QFile::ReadOnly)) {
    QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
    return;
  }

  qDebug() << fn;

  /** DATABASE **/
  if(!connectPGDB())
    return;

  QSqlQuery* q = new QSqlQuery(PGDB->db);
  QString scnt = "SELECT count(id) FROM signals WHERE signal_name = '%1'";

  /** TREATING **/
  int found = 0, lost = 0;
  while(!f.atEnd()) {

    QByteArray line = f.readLine();

    QStringList p = QString(line).split(',');

    QString sName = p.at(0);

    PGDB->execSQL(scnt, q);

    if(!q->next()) {
      ui->textEdit->append(QString("%1.\t%2").arg(++lost).arg(sName));

    }
    else
      ui->textEdit_2->append(QString("%1.\t%2").arg(++found).arg(sName));

    q->finish();

  }

  delete q;
  PGDB->disconnectFromDb();
  f.close();

  qDebug() << "done";

}

void MainWindow::on_bnTestGetSoeg_clicked()
{
  if(!create_tcp_server(502))
    return;

  qDebug() << "server started";

}



bool MainWindow::create_tcp_server(quint16 port)
{
  QString dbname = "cms_db";
  QString dbhost = "localhost";
  quint16 dbport = 5432;
  QString dbuser = "postgres";
  QString dbpass = "postgres";


//    _server = new svtcp::SvTcpServer(_log, false);
//    if(!_server->startServer(port, dbname, dbhost, dbport, dbuser, dbpass)) {
//      qDebug() << QString("%1").arg(_server->lastError());
//      return false;
//    }


    return true;

}

void MainWindow::on_bn1SecCycle_clicked()
{
    if(sec)
    {
        sec->stop();
        delete sec;
        sec = nullptr;
        ui->bn1SecCycle->text() = "1 sec cycle";
    }
    else
    {
        sec = new Sv1SecCycle();
        sec->start();
        ui->bn1SecCycle->text() = "Stop";
        qApp->processEvents();
        qDebug() << "1 sec";
    }
}

void MainWindow::on_bnDigital2_clicked()
{
    QString fn = "/home/user/12700/18.08.2019/system_12 change/model/12_System.prs";

    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    qDebug() << fn;

    QString dPattern("(select({users.admin.devices.database2:can_data_digital}, 'dev_data', 'can_id', %1)");
    QString dsp = "(cell({users.admin.queries.getDiscreteValue:execute(";

    QStringList new_text;
    QString panic = "";

    while(!f.atEnd()) {

      QByteArray line = f.readLine();

      while(true) {

        if(line.contains(dsp.toUtf8())) {

          while(!line.trimmed().endsWith(QString("</value>").toUtf8()))
            line += f.readLine();

          int dsp_start = line.indexOf(dsp);
          int canid_start = dsp_start + dsp.length();
          int canid_end = canid_start;
          QString canid_str = "";

          while(QChar(line.at(canid_end)).isDigit())
            canid_str.append(line.at(canid_end++));


          if(canid_str.trimmed().isEmpty()) {

            panic = QString("Panic! NO NUMBER: %1").arg(QString(line));
//            qDebug() << panic;
//            QMessageBox::critical(this, "", panic, QMessageBox::Ok);

            break;

          }

          int dsp_end = line.indexOf("\")", canid_end) + 1;

         qDebug() << line;
//         qDebug() << canid_str << devindex_str;

         line.replace(dsp_start, dsp_end - dsp_start + 1, QString("[~%1~]").arg(canid_str).toUtf8());

//         qDebug() << line;
//         qDebug() << line;

        }
        else
          break;

      }


      while(true) {

        if(line.contains("[~")) {

          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");

          QString canid_str(line.mid(r_start + 2, r_end - r_start - 2));

          line.replace(r_start, r_end - r_start + 2, QString(dPattern).arg(canid_str).toUtf8());

//          line.replace(QString("[~%1~]").arg(canid_str).toUtf8(), QString("ld('val_%1')").arg(canid_str).toUtf8());

          qDebug() << line;
//          qDebug() <<  ' ';

        }
        else
          break;


      }

      new_text.append(line);

//      if(!panic.isEmpty()) break;

    }

    f.close();


    QFileInfo fi(fn);
    QString newfn = fi.path() + "/d_" + fi.fileName(); //.push_front("_1SDAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    for(QString line: new_text)
      f.write(line.toUtf8());

    f.close();

    qDebug() << "done";

}

void MainWindow::on_bnAnalog2_clicked()
{
    QString fn = "/home/user/12700/18.08.2019/system_12 change/model/12_System.prs";

    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    qDebug() << fn;

    QString aPattern = "(select(filter({users.admin.devices.database2:can_data_analog}, \"contains({can_id}, '%1')\"), 'dev_data', 'dev_index', %2)";
    QString asp = "(cell({users.admin.queries.getAnalogValue:execute(";

    QStringList new_text;
    QString panic = "";

    while(!f.atEnd())
    {

      QByteArray line = f.readLine();

      while(true)
      {
          if(line.contains(asp.toUtf8()))
          {
              while(!line.trimmed().endsWith(QString("</value>").toUtf8()))
              {
                line += '\n' + f.readLine();
              }

//              qDebug() << line;
            int asp_start = line.indexOf(asp);
            int canid_start = asp_start + asp.length();
            int canid_end = canid_start;
            QString canid_str = "";

            while(QChar(line.at(canid_end)).isDigit())
              canid_str.append(line.at(canid_end++));


            if(canid_str.trimmed().isEmpty()) {
              qDebug() << QString("Panic! NO NUMBER: %1").arg(QString(line));
              break;
            }

            int devindex_end = line.indexOf(')', canid_end) - 1;
            int devindex_start = devindex_end;
            QString devindex_str = "";

            while(QChar(line.at(devindex_start)) != QChar(','))
              devindex_str.push_front(line.at(devindex_start--));

            int asp_end = line.indexOf("\")", devindex_end) + 1;

//           qDebug() << line;
  //         qDebug() << canid_str << devindex_str;

           line.replace(asp_start, asp_end - asp_start + 1, QString("[~%1;%2~]").arg(canid_str).arg(devindex_str).toUtf8());

          }
          else break;
      }


      while(true) {

        if(line.contains("[~")) {

            int r_start = line.indexOf("[~");
            int r_end = line.indexOf("~]");

            QString pat(line.mid(r_start, r_end - r_start + 2));
            QStringList sl = QString(line.mid(r_start + 2, r_end - r_start - 2)).split(';');

            if(sl.count() != 2) {
              qDebug() << "panic!";
              break;
            }

            qDebug() << pat << sl.at(0) << sl.at(1);

            line.replace(r_start, r_end - r_start + 2, QString(aPattern).arg(sl.at(0)).arg(sl.at(1)).toUtf8());

        }
        else
          break;


      }

      new_text.append(line);

//      if(!panic.isEmpty()) break;

    }

    f.close();


    QFileInfo fi(fn);
    QString newfn = fi.path() + "/a" + fi.fileName(); //.push_front("_1SDAh_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    for(QString line: new_text)
      f.write(line.toUtf8());

    f.close();

    qDebug() << "done";

}

void MainWindow::on_bnSignal2_clicked()
{
  QString fpath = "/home/user/12700/18.08.2019/system_12 change/model";

  QStringList fnl = QFileDialog::getOpenFileNames(this, "", fpath, "*.prs");

  if(fnl.isEmpty()) return;

  QString sPattern = "select({users.admin.devices.database2:signals_data_rs}, 'value', 'signal_name', '%1')";
//  QString sPattern("st('val_%1', cell({users.admin.queries.getSignalValue:execute(\"%1\")}, \"sdvSignals_data_rsValue\"))");
//  QString aPattern("st('val_%1 _%2', st(%1, %2))");
  for(QString fn: fnl) {

    QFile f(fn);
    if(!f.open(QFile::Text | QFile::ReadOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    qDebug() << fn;

//    QString ssp = "select({users.admin.devices.cms_db:signals_data_rs}";
    QString ssp = "cell({users.admin.queries.getSignalValue:execute(";
    QStringList new_text;
    QString panic = "";

    while(!f.atEnd()) {

      QByteArray line = f.readLine();

      while(true) {

        if(line.contains(ssp.toUtf8())) {

          while(!line.trimmed().endsWith(QString("</value>").toUtf8())) {
            line += '\n' + f.readLine();
          }

          int ssp_start = line.indexOf(ssp);
          int ssp_end = line.indexOf("\"))", ssp_start) + 1;

          int sname_begin = line.indexOf('\"', ssp_start) + 1;
          int sname_end = line.indexOf('\"', sname_begin);

          QString sname_str = line.mid(sname_begin, sname_end - sname_begin);

         qDebug() << sname_str;
//         qDebug() << canid_str << devindex_str;

         line.replace(ssp_start, ssp_end - ssp_start + 1, QString("[~%1~]").arg(sname_str).toUtf8());

//         qDebug() << line;
//         qDebug() << line;

        }
        else
          break;

      }


      while(true) {

        if(line.contains("[~")) {

          int r_start = line.indexOf("[~");
          int r_end = line.indexOf("~]");

          QString sname(line.mid(r_start + 2, r_end - r_start - 2));

          if(!SGNS.contains(sname))
            qDebug() << "no signal" << sname;

//          line.replace(r_start, r_end - r_start + 2, QString(sPattern).arg(sname).arg(SGNS[sname]).toUtf8());
          line.replace(QString("[~%1~]").arg(sname).toUtf8(), QString(sPattern).arg(sname).toUtf8());
//          line.replace(QString("[~%1~]").arg(sname).toUtf8(), QString("ld('val_%1')").arg(sname).toUtf8());


//          qDebug() << line;
//          qDebug() <<  ' ';

        }
        else
          break;


      }

      new_text.append(line);

//      if(!panic.isEmpty()) break;

    }

    f.close();

    QFileInfo fi(fn);
    QString newfn = fi.path() + "/s" + fi.fileName();
//    QString newfn = fi.path() + "/" + fi.fileName().replace(0, 4, "_sda_");

    f.setFileName(newfn);
    if(!f.open(QFile::Text | QFile::WriteOnly)) {

      QMessageBox::critical(this, "", f.errorString(), QMessageBox::Ok);
      return;
    }

    for(QString line: new_text)
      f.write(line.toUtf8());

    f.close();

    qDebug() << "done";

//    qDebug() << "done";

  }
}

void MainWindow::on_bnTestNewSignals_clicked()
{
  if(!connectPGDB())
    return;

  p_test_new_signals_file.setFileName("/home/user/share/T_perem_list.csv");
  if(!p_test_new_signals_file.open(QFile::ReadOnly))
    return;

  qDebug() << "opened!";

  p_test_new_signals_file.seek(0);

  p_tested_signals_file.setFileName("/home/user/share/T_tested_list.csv");
  if(!p_tested_signals_file.open(QFile::WriteOnly))
    return;

  qDebug() << "opened tested!";

}

void MainWindow::on_bnNextNewSignal_clicked()
{
    if(!p_test_new_signals_file.isOpen()) {
      qDebug() << "closed!";
      return;
    }
qDebug() << signal_name;
    if(!p_test_new_signals_file.atEnd()) {

      QString s = "%1;%2";
      if(!signal_name.isEmpty()) {

        if(ui->radioOk->isChecked()) s = s.arg(signal_name).arg("OK");
        else if(ui->radio525->isChecked()) s = s.arg(signal_name).arg("5%25");
        else if(ui->radioHladon->isChecked()) s = s.arg(signal_name).arg("отображается в хладоне");
        else if(ui->radioEdit->isChecked()) s = s.arg(signal_name).arg(ui->editEdit->text());

        p_tested_signals_file.write(s.toUtf8());

      }

      QSqlError err = PGDB->execSQL(QString("update signals_data set value = 0.0 where signal_index in "
                                            "(select signal_index from signals where signal_name like '%\\_%1\\_T%')").arg(signal_name));

      if(err.type() != QSqlError::NoError) {
        qDebug() << err.text();
        return;
      }


      signal_name = QString(p_test_new_signals_file.readLine()).trimmed();

      ui->lblSignalName->setText(signal_name);

      err = PGDB->execSQL(QString("update signals_data set value = 1.0 where signal_index in "
                                    "(select signal_index from signals where signal_name like '%\\_%1\\_T%')").arg(signal_name));

      if(err.type() != QSqlError::NoError) {
        qDebug() << err.text();
        return;

      }
    }
    else {

      p_test_new_signals_file.close();
      delete PGDB;

    }

}

void MainWindow::on_bnCloseTestNewSignals_clicked()
{
  p_test_new_signals_file.close();
  p_tested_signals_file.close();
  delete PGDB;
}

void MainWindow::on_bnAddPlacements_clicked()
{
  if(!connectPGDB("cms_db_modified"))
    return;

  QFile placements("/home/user/tmp/placements.csv");
  if(!placements.open(QFile::ReadOnly))
    return;

  qDebug() << "opened!";

  int idx = 0;
  QSqlQuery *q = new QSqlQuery(PGDB->db);

  while(!placements.atEnd()) {

    QString placement = QString(placements.readLine().trimmed());

    QString qs1 = QString("select count(*) as cnt from placements where placement_name = '%1'").arg(placement);

    QSqlError err = PGDB->execSQL(qs1, q);

    if(err.type() != QSqlError::NoError) {

      qDebug() << err.text();
      q->finish();
      delete q;
      break;
    }

    q->next();
    if(q->value("cnt").toInt() == 0) {

      err = PGDB->execSQL(QString("INSERT INTO placements(index, placement_name) VALUES (%1, '%2');").arg(idx).arg(placement));
      if(err.type() != QSqlError::NoError) {

        qDebug() << err.text();
        q->finish();
        delete q;
        break;
      }

      idx++;
      qDebug() << placement;

    }

    q->finish();

  }

  delete q;

  placements.close();
  qDebug() << "finished";

}

void MainWindow::on_bnAddSensors_clicked()
{
  if(!connectPGDB("cms_db_modified"))
    return;

  QSqlError err;

  int i = 0;
  foreach (quint8 STK, SIGNALS_TABLE.keys()) {

    signals_by_detector* signals_map = SIGNALS_TABLE.value(STK);


    foreach (int sensor_num, signals_map->keys()) {

      QString signal_name = signals_map->value(sensor_num);

      err = PGDB->execSQL(QString("update signals set factor_value = %1 where signal_name = '%2'").arg(STK).arg(signal_name));
      if(err.type() != QSqlError::NoError) {

        qDebug() << err.text();
        return;

      }

      err = PGDB->execSQL(QString("INSERT INTO sensors(sensor_number, device_index) select %1, 0 "
                         "where not exists (select 1 from sensors where sensor_number = %1)").arg(sensor_num));

      if(err.type() != QSqlError::NoError) {

        qDebug() << err.text();
        return;

      }

      qDebug() << STK << sensor_num << signal_name;


    }

  }
qDebug() << "finished";

}


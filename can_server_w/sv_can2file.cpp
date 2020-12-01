#include "sv_can2file.h"

SvCan2File::SvCan2File(QObject *parent) : QObject(parent)
{

}

SvCan2File::~SvCan2File()
{
  m_currrent_file.close();
}

bool SvCan2File::init(const AppConfig& cfg)
{
  config = cfg;

  m_path = QDir(config.path);

  if(!m_path.exists()) {

    m_last_error = QString("Путь не найден: %1").arg(config.path);
    return false;

  }

  m_total_duration_msec = (config.file_duration.date().month() * 30 * 86400 +
                           config.file_duration.date().day() * 86400 +
                           config.file_duration.time().hour() * 3600 +
                           config.file_duration.time().minute() * 60 +
                           config.file_duration.time().second()) * 1000;

  m_file_duration_msec = (config.file_duration.time().hour() * 3600 +
                          config.file_duration.time().minute() * 60 +
                          config.file_duration.time().second()) * 1000;

  m_new_file = true;
  m_total_finished = false;

  m_total_epoch = QDateTime::currentMSecsSinceEpoch();
  m_file_epoch = m_total_epoch;

  return true;
}

bool SvCan2File::write(quint8 port_idx, quint16 can_id, qint64 data)
{
  qint64 currentMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();

  /**** проверяем, задано ли полное время записи *****/
  if(m_total_duration_msec)
  {
    /* если задано полное время записи, то           *
    * проверяем не истекло ли полное время записи        *
    * если истекло, то выходим и закрываем текущий файл  */

    if(currentMSecsSinceEpoch > m_total_epoch + m_total_duration_msec)
    {
      if(m_currrent_file.isOpen())
        m_currrent_file.close();

      m_new_file = true;
      m_total_finished = true;

      m_last_error = QString("Total record time %1 elapsed")
                          .arg(config.total_duration.toString("MM.dd hh:mm:ss"));
      return false;

    }
  }

  /****** проверяем не истекло ли время записи текущего файла *****/
  if(currentMSecsSinceEpoch > m_file_epoch + m_file_duration_msec)
  {
    /* close output */
    if(m_currrent_file.isOpen())
      m_currrent_file.close();

    m_new_file = true;

  }

  /******** открываем новый файл для записи **********/
  if(m_new_file)
  {
    QString current_file_name = QFileInfo(m_path, QString("%1_%2.arch")
                                                  .arg(QDate::currentDate().toString("ddMMyy"))
                                                  .arg(QTime::currentTime().toString("hhmmss")))
                            .canonicalPath();

    m_currrent_file.setFileName(current_file_name);
    if(!m_currrent_file.open(QIODevice::WriteOnly))
    {
      m_last_error = m_currrent_file.errorString();
      m_new_file = true;

      return false;
    }

    m_file_epoch = QDateTime::currentMSecsSinceEpoch();
    m_new_file = false;

  }

  m_currrent_file.write(&port_idx, sizeof(quint8));
  m_currrent_file.write(&can_id, sizeof(quint16));
  m_currrent_file.write(&data, sizeof(qint64));

}

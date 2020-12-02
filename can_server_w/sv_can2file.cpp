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

  // создаем файл, чтобы убедиться, что есть права на создание и запись файлов
  // также этот файл нужен дальше для функции statfs
  m_statfs_file = QFileInfo(m_path, QString("%1_%2.statfs")
                            .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                            .arg(QTime::currentTime().toString("hh_mm_ss")))
      .absoluteFilePath();

  m_currrent_file.setFileName(m_statfs_file);
  if(!m_currrent_file.open(QIODevice::WriteOnly)) {

    m_last_error.clear();
    m_last_error.append(QString("Ошибка открытия файла %1: ").arg(m_statfs_file));
    m_last_error.append(m_currrent_file.errorString());
    return false;

  }
  else if(m_currrent_file.write("test") == -1) {

    m_last_error.clear();
    m_last_error.append("Ошибка записи в файл: ").append(m_currrent_file.errorString());
    return false;

  }

  m_total_duration_msec = (config.total_duration.months * 30 * 86400 +
                           config.total_duration.days * 86400 +
                           config.total_duration.time.hour() * 3600 +
                           config.total_duration.time.minute() * 60 +
                           config.total_duration.time.second()) * 1000;

  m_file_duration_msec = (config.file_duration.time().hour() * 3600 +
                          config.file_duration.time().minute() * 60 +
                          config.file_duration.time().second()) * 1000;

  // вычисляем размер одного файла (примерно), чтобы учитывать свободное место
  // считаем, исходя из того, что  запись идет каждую 1 мсек.
  m_file_space_required = m_file_duration_msec *
                          config.can_id_list.split(",").count() *
                          (sizeof(QDateTime) + sizeof(quint8) + sizeof(quint16) + sizeof(qint64));

  m_file_space_required *= 10; // берем с 10-кратным запасом

  m_new_file = true;
  m_total_finished = false;

  m_total_epoch = QDateTime::currentMSecsSinceEpoch();
  m_file_epoch = m_total_epoch;

  m_stream.setVersion(QDataStream::Qt_5_5);

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
    // проверяем, достаточно ли места на диске для записи файла
    if(get_fs_free(m_statfs_file.toStdString().c_str()) < (long)m_file_space_required) {

      m_last_error = "Место для записи архива данных can шины исчерпано";
      m_total_finished = true;
      return false;

    }


    QString current_file_name = QFileInfo(m_path, QString("%1_%2.arch")
                                                  .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                                                  .arg(QTime::currentTime().toString("hh_mm_ss")))
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

    m_stream.setDevice(&m_currrent_file);

  }

  m_stream << QDateTime::currentDateTime() << port_idx << can_id << data;

  return true;

}


inline long get_fs_size(const char *anyfile)
{
  struct statfs buf;
  statfs(anyfile, &buf);
  return buf.f_blocks * blksize(anyfile);
}

inline long get_fs_free(const char *anyfile)
{
  struct statfs buf;
  statfs(anyfile, &buf);
  return buf.f_bfree * blksize(anyfile);
}

inline size_t blksize(const char *anyfile)
{
  struct stat st;
  stat(anyfile, &st);
  return st.st_blksize;
}

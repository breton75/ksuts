#ifndef SVCAN2FILE_H
#define SVCAN2FILE_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "time.h"
#include <QDateTime>

#include "mainwindow.h"

class SvCan2File : public QObject
{
  Q_OBJECT
public:
  explicit SvCan2File(QObject *parent = 0);
  ~SvCan2File();

  bool init(const AppConfig& config);
  bool write(quint8 port_idx, quint16 can_id, qint64 data);

  const QString lastError() const { return m_last_error; }

  const bool totalFinished() const { return m_total_finished; }

private:
  QFile m_currrent_file;
  QDir  m_path;

  QString m_last_error = "";

  qint64 m_file_duration_msec;
  qint64 m_total_duration_msec;
  qint64 m_total_epoch;
  qint64 m_file_epoch;
  bool   m_new_file;

  bool   m_total_finished;

  AppConfig config;

signals:

public slots:
};

#endif // SVCAN2FILE_H

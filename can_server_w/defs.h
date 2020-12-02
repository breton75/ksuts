#ifndef DEFS
#define DEFS

#include <QtCore>

#define OPTION_DB_HOST                   "db_host"
#define OPTION_DB_PORT                   "db_port"
#define OPTION_DB_NAME                   "db_name"
#define OPTION_DB_USER                   "db_user"
#define OPTION_DB_PASS                   "db_pass"
#define OPTION_LOG_CAN_DATA              "log_can_data"
#define OPTION_TOTAL_DURATION            "total_duration"
#define OPTION_FILE_DURATION             "file_duration"
#define OPTION_PATH                      "path"
#define OPTION_FILE_NAME_PATTERN         "file_name_pattern"
#define OPTION_CAN_ID_LIST               "can_id_list"

struct Interval {
  quint16 years;
  quint16 months;
  quint16 days;

  QTime   time;

  QString toString(const QString& format) {

    QString str = format;

    str.replace("yyyy", QString("%1").arg(years, 4))
        .replace("yy", QString("%1").arg(years % 100, 2))
        .replace("MM", QString("%1").arg(months, 2))
        .replace("dd", QString("%1").arg(days, 2))
        .replace("hh", QString("%1").arg(time.hour(), 2))
        .replace("mm", QString("%1").arg(time.minute(), 2))
        .replace("ss", QString("%1").arg(time.second(), 2));

    return str;
  }
};

struct AppConfig {
    QString   db_name;
    QString   db_host;
    quint16   db_port;
    QString   db_user;
    QString   db_pass;
    bool      log_can_data;
    Interval  total_duration;
    QDateTime file_duration;
    QString   path;
    QString   file_name_pattern;
    QString   can_id_list;
};

#endif // DEFS


#include "sv_1seccycle.h"

Sv1SecCycle::Sv1SecCycle()
{

}

void Sv1SecCycle::run()
{
    _PGDB = new SvPGDB();

    QString db = "cms_db";
    QString host = "localhost";
    QString user = "postgres";
    QString pass = "postgres";

    _PGDB->setConnectionParams(db, host, 5432, user, pass);

    QSqlError serr = _PGDB->connectToDB();

    if((serr.type() != QSqlError::NoError)) {

      qDebug() << serr.databaseText();
      return;
    }
    else qDebug() << "connected!";

    _is_active = true;
    _finished = false;
    bool odd = true;
    while(_is_active)
    {

        serr = _PGDB->execSQL(QString("update digital_data set dev_data = %1 where can_id = 960").arg(odd ? 0x7FFFFFFFFFFFFFFF : 0));

        if(serr.type() != QSqlError::NoError)
          qDebug() << serr.text();

        qsrand(QTime::currentTime().msecsSinceStartOfDay());
        int r1 = qrand() % 100;
        int r2 = qrand() % 10;
        QString sql = QString("update analog_data set dev_data = %1 where can_id = 989 and dev_index = 0").arg(odd ? float(r1) / 100.0 * float(r2) : 0);
        serr = _PGDB->execSQL(sql);

//        qDebug() << sql;
        if(serr.type() != QSqlError::NoError)
          qDebug() << serr.text();

        odd = !odd;

        msleep(1000);

    }
    _finished = true;

}

void Sv1SecCycle::stop()
{
    _is_active = false;

    while(!_finished) qApp->processEvents();

    _PGDB->disconnectFromDb();
}


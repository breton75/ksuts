#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QtGui>
#include <QMessageBox>
#include <QWidget>
#include <QTextStream>

#include <iostream>
//#include <QCoreApplication>

//#include<stdio.h>
//#include <sys/types.h>
//#include <unistd.h>

QMutex mut;

void qDebugHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_UNUSED(type);
  Q_UNUSED(context);

  mut.lock();
    if(MainWindow::log)
    {
      MainWindow::log->append(msg);
    }

    mut.unlock();
}

int main(int argc, char *argv[])
{
  // запрос версии для монитора
  if((argc > 1) && (QString(argv[1]).trimmed() == "-v")) {
    std::cout << QString(APP_VERSION).toStdString().c_str() << std::endl;
    return 0;
  }

  qInstallMessageHandler(qDebugHandler);

    QApplication a(argc, argv);

    MainWindow w;

    //qDebug() << argv[0];
    // порт постгрес
    if(argc > 1) {
        int pgPort = atoi(argv[1]);
        if(pgPort > 5000) w.setPGPort(pgPort);
        else {
            qDebug() << "Задан некорректный номер порта сервера постгрес:" << pgPort;
            qDebug() << "Оставлен порт по умолчанию: 5432";
        }
    }

    // инициализация
    if(!w.init()) {
        qDebug() << "Нажмите ENTER для выхода...";
        getchar();
        a.quit();//exit(-1);
        return -1;
    }

//    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QtGui>
#include <QMessageBox>
#include <QWidget>

#include <QTextStream>
//#include <QCoreApplication>

//#include<stdio.h>
//#include <sys/types.h>
//#include <unistd.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc > 3){
      qWarning() << "Необходимо задать ip адреса и порты для can0 и can1 в формате: 192.168.1.1:10003 192.168.1.1:10003";
      a.exit(-1);
    }

    MainWindow w;

    //qDebug() << argv[0];
    // порт постгрес
/**    if(argc > 1) {
        int pgPort = atoi(argv[1]);
        if(pgPort > 5000) w.setPGPort(pgPort);
        else {
            qDebug() << "Задан некорректный номер порта сервера постгрес:" << pgPort;
            qDebug() << "Оставлен порт по умолчанию: 5432";
        }
    }
**/

    if(!w.setCanHosts(argv[1], argv[2]))
    {
      qWarning() << "Необходимо задать ip адреса и порты для can0 и can1 в формате: 192.168.1.1:10003 192.168.1.1:10003";
      a.exit(-1);
    }

//    return 0;

    // инициализация
    if(!w.init()) {
        qDebug() << "Нажмите ENTER для выхода...";
        getchar();
        a.quit();//exit(-1);
        return -1;
    }

    w.show();
    return a.exec();
}

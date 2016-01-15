#include "mainwindow.h"
#include <QApplication>

#define PROTECTION_ON

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool canRun=true;
#ifdef PROTECTION_ON
    if(QDate::currentDate()>QDate(2016, 1, 1))
        canRun=false;
#endif

    if(canRun)
    {
        MainWindow w;
        w.show();
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Critical,
                           QObject::trUtf8("Запуск невозможен"),
                           QObject::trUtf8("Данная версия программы более не поддерживается.\n"
                                  "Обратитесь к разработчику для получения новой версии."),
                           QMessageBox::Ok
                           );
        msgBox.exec();
        return 1;
    }

    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

#define PROTECTION_ON

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool canRun=true;
#ifdef PROTECTION_ON
    if(QDate::currentDate()>QDate(2016, 2, 20))
        canRun=false;
#endif

    MainWindow w;
    QMessageBox msgBox(QMessageBox::Critical,
                       QObject::trUtf8("Запуск невозможен"),
                       QObject::trUtf8("Данная версия программы более не поддерживается.\n"
                              "Обратитесь к разработчику для получения новой версии."),
                       QMessageBox::Ok
                       );
    if(canRun)
    {
        w.show();
    }
    else
    {
        msgBox.show();
    }

    return a.exec();
}

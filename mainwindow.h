#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defines.h"
#include "databasewidget.h"
#include "address.h"
#include "excelwidget.h"
#include "debugwidget.h"

typedef QList< Address > ListAddress;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void windowClosed();
    void toDebug(QString, QString);

protected:
    void closeEvent(QCloseEvent *);

public slots:
    void onBaseOpenTriggered();
    void onExcelOpenTriggered();
    void onErrorOccured(QString nameObject, int code, QString errorDesc);

private slots:
    void on__pushButtonOpen_clicked();
    void on__pushButtonOpenBase_clicked();
    void on__pushButtonSearch_clicked();

    void save();
    void onStartSearching(const QString &sheet);
    void onFinishSearching(const QString &sheet);

    void on__pushButtonWait_clicked();

private:
    Ui::MainWindow *_ui;
    DatabaseWidget *_dbw;
    ExcelWidget *_excel;
    DebugWidget *_debugWidget;
};

#endif // MAINWINDOW_H

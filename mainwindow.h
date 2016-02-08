#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "defines.h"
#include "databasewidget.h"
#include "address.h"
#include "excelwidget.h"
#include "debugwidget.h"
#include "helpbrowser.h"

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
    void keyPressEvent(QKeyEvent *pe);

public slots:
    void show();
    void onBaseOpenTriggered();
    void onExcelOpenTriggered();
    void onErrorOccured(QString nameObject, int code, QString errorDesc);
    void onMessageReady(QString mes);

private slots:
    void on__pushButtonOpen_clicked();
    void on__pushButtonOpenBase_clicked();
    void on__pushButtonSearch_clicked();
    void on__pushButtonWait_clicked();
    void on__pushButtonCloseTab_clicked();

    void save();
    void onStartSearching(const QString &sheet);
    void onFinishSearching(const QString &sheet);
    void onSheetParsed(const QString &sheet);
    void onFileOpening(const QString &fname);
    void onFileOpened(const QString &fname);
    void onFileError(const QString &fname);
    void showAbout();
    void showHelp();

    void on__pushButtonStop_clicked();

private:
    Ui::MainWindow *_ui;
    DatabaseWidget *_dbw;
    ExcelWidget *_excel;
    DebugWidget *_debugWidget;
    HelpBrowser *_helpBrowser;
};

#endif // MAINWINDOW_H

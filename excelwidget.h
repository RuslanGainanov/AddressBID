#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QScopedPointer>
#include <QProgressBar>
#include <QFile>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QAxObject>
#include <shlobj.h> //для использования QAxObject в отдельном потоке

#include "defines.h"
#include "address.h"
#include "xlsparser.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tabwidget.h"

const int MAX_OPEN_IN_ROWS=10;

namespace Ui {
class ExcelWidget;
}

class ExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExcelWidget(QWidget *parent = 0);
    ~ExcelWidget();

public slots:
    void open();

signals:
    void headReaded(QString sheet, QStringList head);
    void rowReaded(QString sheet, int row);
    void countRows(QString sheet, int count);
    void sheetsReaded(QStringList sheets);

    void headParsed(QString sheet, MapAddressElementPosition head);
    void rowParsed(QString sheet, int row);

    void working(); //начало чтения файла
    void finished(); //чтение файла окончено

    void toDebug(QString objName, QString mes);
    void messageReady(QString);
    void errorOccured(QString nameObject, int code, QString errorDesc);

private slots:
    //parser signal-slots
    void onParseRow(QString sheet, int rowNumber, Address a);
    void onHeadParsed(QString sheet, MapAddressElementPosition head);
    void onFinishParser();

    void runThread(QString openFilename);
    QVariant openExcelFile(QString filename, int maxCount);
    void onProcessOfOpenFinished();

private:
    Ui::ExcelWidget             *_ui;
    XlsParser                   *_xlsParser;
    QMap<QString, TableModel *>  _data;
    QFutureWatcher<QVariant>     _futureWatcher;
    QProgressDialog              _dialog;
};

#endif // EXCELWIDGET_H

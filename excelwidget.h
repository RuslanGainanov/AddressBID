#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QScopedPointer>
#include <QFile>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QAxObject>
#include <QMessageBox>
#include <shlobj.h> //для использования QAxObject в отдельном потоке

#include "defines.h"
#include "address.h"
#include "xlsparser.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tabwidget.h"
#include "parseexcelwidget.h"

const int MAX_OPEN_IN_ROWS=100;

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
    void parse();

signals:
    void headReaded(QString sheet, MapAddressElementPosition head);
    void rowReaded(QString sheet, int nRow, QStringList row);
    void countRows(QString sheet, int count);
    void sheetsReaded(QStringList sheets);
    void isOneColumn(bool);
    void parserFinished();

    void headParsed(QString sheet, MapAddressElementPosition head);
    void rowParsed(QString sheet, int row);
    void sheetParsed(QString sheet);

    void working(); //начало чтения файла
    void finished(); //чтение файла окончено

    void toDebug(QString objName, QString mes);
    void messageReady(QString);
    void errorOccured(QString nameObject, int code, QString errorDesc);

private slots:
    void onRowRead(const QString &sheet, const int &nRow, QStringList &row);
    void onHeadRead(const QString &sheet, QStringList &head);
    void onSheetRead(const QString &sheet);
    void onHideColumn(const QString &sheet, int column);

    //parser signal-slots
    void onRowParsed(QString sheet, int nRow, Address a);
    void onHeadParsed(QString sheet, MapAddressElementPosition head);
    void onSheetParsed(QString sheet);
    void onFinishParser();
    void onAppendColumn(int nCol, QString nameCol);
    void onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName);

    void onProcessOfOpenFinished();//после того окончили с открытием excel документа

private:
    Ui::ExcelWidget             *_ui;
    XlsParser                   *_parser;
    QHash<QString, TableModel *> _data;
    QHash<QString, TableView *>  _views;
    QHash<QString, int>          _sheetIndex;
    QFutureWatcher<QVariant>     _futureWatcher;
    QProgressDialog              _dialog;
    bool                         _isOneColumn;
    QMap<QString, MapAddressElementPosition> _mapHead;
    QMap<QString, MapAddressElementPosition> _mapPHead;
    QThread *_thread;
    QHash<QString, int> _countParsedRow;
    QHash<QString, int> _countRow;

    void runThreadOpen(QString openFilename);
    QVariant openExcelFile(QString filename, int maxCount);
    void runThreadParsing();
};

#endif // EXCELWIDGET_H

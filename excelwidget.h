#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QScopedPointer>
#include "xlsworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"
#include "xlsparser.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tabwidget.h"

const int MAX_OPEN_IN_ROWS=0;

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
    void runThread(QString openFilename);

signals:
    void headReaded(int sheet, QStringList head);
    void rowReaded(int sheet, int row);
    void readedRows(int sheet, int count);
    void countRows(int sheet, int count);
    void sheetsReaded(QMap<int, QString> sheets);
    void sheetsAllReaded(QStringList sheets);

    void headParsed(int sheet, MapAddressElementPosition head);
    void rowParsed(int sheet, int row);

    void working(); //начало чтения файла
    void finished(); //чтение файла окончено

    void toDebug(QString);
    void toDebug(QString objName, QString mes);
    void messageReady(QString);
    void errorOccured(QString nameObject, int code, QString errorDesc);

    void removeEmptySheets(QVector<int> sheetNumbers);

private slots:
    //worker signal-slots
    void onReadHead(int sheet, QStringList head);
    void onReadRow(int sheet, int row, QStringList listRow);
    void onReadRows(int sheet, int count);
    void onCountRows(int sheet, int count);
    void onSheetsAllReaded(QStringList sheets); //после чтения всех листов (даже тех которые пусты)
    void onSheetsReaded(const QMap<int, QString> &sheets);
    void onFinishWorker();

    //parser signal-slots
    void onParseRow(int sheet, int rowNumber, Address a);
    void onHeadParsed(int sheet, MapAddressElementPosition head);
    void onFinishParser();

    void onRemoveEmptySheets(QVector<int> numb);

    void on__pushButtonOpen_clicked();

private:
    Ui::ExcelWidget *_ui;
    XlsWorker *_xls;
    QThread *_thread;
    Parser *_parser;
    XlsParser *_xlsParser;
    QMap<int, QString> _sheets;
    QMap<int, QStringList> _sheetsHead;

    QMap<int, TableModel *> _data;
};

#endif // EXCELWIDGET_H

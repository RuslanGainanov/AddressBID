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
#include <QStyledItemDelegate>
#include <QPainter>
#include <shlobj.h> //для использования QAxObject в отдельном потоке

#include "defines.h"
#include "address.h"
#include "xlsparser.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tabwidget.h"
#include "parseexcelwidget.h"
#include "itemselectionmodel.h"
#include "database.h"

typedef QList< Address > ListAddress;

#define HIDE_PARSED_COLUMNS 1

const int MAX_OPEN_IN_ROWS=10;
const QString FoundedColor = "#33FF66"; //цвет строки для которой найден BID в базе
const QString NotFoundedColor = "#FFA07A"; //цвет строки для которой не найден BID в базе
const QString RepeatFoundedColor = "#FFCC33"; //цвет строки для которой найдено >1 BID в базе
const QString LogFileName = "Log1.csv";

class SimpleDelegate : public QStyledItemDelegate {
public:
    SimpleDelegate(QBrush b=QBrush(), QObject* pobj = 0) : QStyledItemDelegate(pobj)
    {
        _b=b;
    }

    void paint(QPainter*                   pPainter,
               const QStyleOptionViewItem& option,
               const QModelIndex&          index
              ) const
    {
        if(!index.isValid())
        {
            QStyledItemDelegate::paint(pPainter, option, index);
            return;
        }
        QRect rect = option.rect;
        pPainter->setBrush(_b);
        pPainter->drawRect(rect);
        QStyledItemDelegate::paint(pPainter, option, index);
    }

private:
    QBrush _b;
};

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
    void search();
    void closeLog();

signals:
    void headReaded(QString sheet, MapAddressElementPosition head);
    void rowReaded(QString sheet, int nRow, QStringList row);
    void countRows(QString sheet, int count);
    void sheetsReaded(QStringList sheets);
    void isOneColumn(bool);

    void rowParsed(QString sheet, int row);
    void sheetParsed(QString sheet);

    void working(); //начало чтения файла
    void finished(); //чтение файла окончено

    void searching();

    void toDebug(QString objName, QString mes);
    void toFile(QString objName, QString mes);

    void findRowInBase(QString sheetName, int nRow, Address addr);

    void messageReady(QString);
    void errorOccured(QString nameObject, int code, QString errorDesc);

    void currentRowChanged(QString sheet, int nRow, MapAEValue data); //срабатывает по изменении строки в модели

private slots:
    void onRowRead(const QString &sheet, const int &nRow, QStringList &row);
    void onHeadRead(const QString &sheet, QStringList &head);
    void onSheetRead(const QString &sheet);
    void onHideColumn(const QString &sheet, int column);

    //parser signal-slots
    void onRowParsed(QString sheet, int nRow, Address a);
    void onSheetParsed(QString sheet);
    void onFinishParser();
    void onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName);

//    void onProcessOfSearchFinished();
    void onFile(QString objName, QString mes);
    void onFounedAddress(QString sheetName, int nRow, Address addr);
    void onNotFounedAddress(QString sheetName, int nRow, Address addr);

    void onProcessOfOpenFinished();//после того окончили с открытием excel документа
    void onRemoveRow(QString sheet, int nRow);
    void onCurrentRowChanged();
    void onCurrentRowChanged(QString sheet, int nRow, QStringList row);
    void onTableDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
    void onParsedDataChanged(QString sheet, int nRow, MapAEValue row);

    void on__lineEditNewAddr_returnPressed();
    void on_pushButton_clicked();
    void onAddNewAddr(QString addr);

private:
    Ui::ExcelWidget             *_ui;
    XlsParser                   *_parser;
    QHash<QString, TableModel *> _data;
    QHash<QString, TableView *>  _views;
    QHash<QString, ItemSelectionModel *>  _selections;
    QHash<QString, int>          _sheetIndex;
    QFutureWatcher<QVariant>     _futureWatcher;
    QFutureWatcher<ListAddress>     _futureWatcherS;
    QMap<QString, MapAddressElementPosition> _mapHead;
    QMap<QString, MapAddressElementPosition> _mapPHead;
    QThread *_thread;
    QHash<QString, int> _countParsedRow;
    QHash<QString, int> _countRow;
    QHash<QString, int> _editedRow; //редактируемая пользователем строка
    QHash<QString, int> _countRepatingRow; //количество повторяющихся строк (строк для которых найдено более одного совпадения в базе)
    QHash<QString, int> _insertedRowAfterSearch; //номер вставляемой строки при повторе
    QString _searchingSheetName;
    SimpleDelegate *_delegateFounded;
    SimpleDelegate *_delegateNotFounded;
    SimpleDelegate *_delegateRepeatFounded;
    QMap<QString, ListAddress > _data2;
//    ExcelDocument _data;
//    QThread *_thread;
    QFile _logFile;
    QTextStream _logStream;
    QVariant openExcelFile(QString filename, int maxCount);
    QVariant openCsvFile(QString filename, int maxCountRows);

    void runThreadParsing();
    void runThreadOpen(QString openFilename);
};

#endif // EXCELWIDGET_H

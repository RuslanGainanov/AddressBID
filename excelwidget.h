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
#include <QTableView>
#include <QItemSelectionModel>
#include <shlobj.h> //для использования QAxObject в отдельном потоке

#include "defines.h"
#include "address.h"
#include "xlsparser.h"
#include "tablemodel.h"
#include "parseexcelwidget.h"
#include "database.h"
#include "simpledelegate.h"
#include "searcher.h"
#include "parseexcelwidget.h"

#define HIDE_PARSED_COLUMNS 1

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
    QString getCurrentTab();
    int getCountTab();
    ParseExcelWidget *getParseWidget()
    {
        return _parseWidget;
    }

public slots:
    void open();
    void search();
    void stopSearch();
    bool closeTab(); //если есть еще вкладки, то true
    bool save(); //если успешно сохранено, то true
    void waitSearchThread();

signals:
    void headReaded(QString sheet, MapAddressElementPosition head);
    void rowReaded(QString sheet, int nRow, QStringList row);
    void countRows(QString sheet, int count);
    void sheetsReaded(QStringList sheets);

    void rowParsed(QString sheet, int row);
    void sheetParsed(QString sheet);

    void opening(QString fname); //начало чтения файла
    void openFinished(QString fname); //чтение файла окончено
    void openError(QString fname); //чтение файла окончено c ошибкой

    void searching(QString sheet); //поиск начат
    void searchFinished(QString sheet); //поиск в базе окончен

    void saving(QString sheet, QString file); //сохранение начато
    void saveFinished(QString sheet, QString file); //сохранение завершено

    void toDebug(QString objName, QString mes);

    void searchInBase(QString sheetName, int nRow, Address addr);
    void searchInBase2(QString sheetName, int nRow, Address addr, bool findIt);
    void startSearchThread(QThread::Priority);
    void finishSearchThread();

    void messageReady(QString);
    void errorOccured(QString nameObject, int code, QString errorDesc);

    void currentRowChanged(QString sheet, int nRow, MapAEValue data); //срабатывает по изменении строки в модели

    void objectDeleted();

private slots:
    void onRowRead(const QString &sheet, const int &nRow, QStringList &row);
    bool onHeadRead(const QString &sheet, QStringList &head);
    void onSheetRead(const QString &sheet);
    void onHideColumn(const QString &sheet, int column);


    void onRowParsed(QString sheet, int nRow, Address a); //parser signal-slots
    void onSheetParsed(QString sheet);
    void onSearchFinished(QString sheet);
    void onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName);

//    void onProcessOfSearchFinished();
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
    ParseExcelWidget            *_parseWidget;
    QHash<QString, TableModel *> _models;
    QHash<QString, QTableView *>  _views;
    QHash<QString, QItemSelectionModel *>  _selections;
//    QHash<QString, int>          _sheetIndex;
    QFutureWatcher<QVariant>     _futureWatcher;
    QFutureWatcher<ListAddress>     _futureWatcherS;
    QMap<QString, MapAddressElementPosition> _mapHead;
    QMap<QString, MapAddressElementPosition> _mapPHead;
    QThread *_thread;
    QHash<QString, int> _countParsedRow;
    QHash<QString, int> _countRow;
    QHash<QString, int> _editedRow; //редактируемая пользователем строка
    QHash<QString, int> _countRepatingRow; //количество повторяющихся строк (строк для которых найдено более одного совпадения в базе)
    QHash<QString, QSet<int> > _searchingRows; //номер строк, которые ищутся в данный момент в БД
    QHash<QString, int> _insertedRowAfterSearch; //номер вставляемой строки при повторе
//    QString _searchingSheetName;
    SimpleDelegate *_delegateFounded;
    SimpleDelegate *_delegateNotFounded;
    SimpleDelegate *_delegateRepeatFounded;
    QMap<QString, ListAddress > _data2;
    QString _openFilename;
//    ExcelDocument _data;
//    QThread *_thread;

    QVariant openExcelFile(QString filename, int maxCount);
    QVariant openCsvFile(QString filename, int maxCountRows);

    bool saveToCsv(const QString &filename, const QString &sheet);
    bool saveToExcel(const QString &filename, const QString &sheetName);

    bool runThreadSave(const QString &filename, const QString &sheetName);
    void runThreadOpen(QString openFilename);

    void removeSheet(QString &sheet);
};

#endif // EXCELWIDGET_H

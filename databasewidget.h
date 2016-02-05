#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QMessageBox>
#include "database.h"
#include "defines.h"
#include "csvworker.h"
#include "xlsparser.h"

namespace Ui {
class DatabaseWidget;
}

// ********************* class DatabaseWidget ********************
/**
 * \class DatabaseWidget
 * \brief Класс для работы пользователя с БД
 */
class DatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatabaseWidget(QWidget *parent = 0);
    ~DatabaseWidget();
    Database *getDatabase()
    {
        return _db;
    }

public slots:
    bool open();
    void openExisting();
    void openExisting(QString fname);
    void clear();
    void viewInfo();
    void waitSearch();

private slots:
    void onReadRow(int row);
    void onReadRows(int rows);
    void onParseRow(int row);
    void onCountRow(int count);
    void onOpenBase();
    void onBaseOpened();
    void readCsvBase(QString openFilename);
    void onFindButtonClicked();
    void onParseButtonClicked();
    void onClearButtonClicked();
    void onSelectedRows(int count);

    void on__pushButtonOpen_clicked();
    void on__pushButtonLoadOld_clicked();
    void onProcessOfOpenFinished();
    void onProcessOfParsingFinished();

    void onToDebug(QString obj, QString mes);

    void on__pushButtonFindParsAddr_clicked();
    void on__pushButtonFindStrAddr_clicked();
    void on__pushButtonClear_clicked();

signals:
    void toDebug(QString, QString);
    void exitSignal();
    void selectAddress(Address a);

private:
    Ui::DatabaseWidget             *_ui;
    Database                       *_db;
    XlsParser                      *_parser;
    QFutureWatcher<ListAddress>     _futureWatcher;
    QFutureWatcher<void>            _futureWatcherParser;
    ListAddress                     _addrs;
    QString                         _csvFileName;
    QThread                         _thread;

    void connectModelWithView(QSqlTableModel *model);
//    void parsingAddress(Address &a);

};

#endif // DATABASEWIDGET_H

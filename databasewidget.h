#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include "database.h"
#include "defines.h"

#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QMessageBox>
#include <assert.h>
#include "csvworker.h"

namespace Ui {
class DatabaseWidget;
}

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
    void open();
    void openExisting();
    void clear();
    void viewInfo();

private slots:
    void onReadRow(int row);
    void onReadRows(int rows);
    void onParseRow(int row);
    void onCountRow(int count);
    void onOpenBase();
    void onBaseOpened();
    void readCsvBase(QString openFilename);

    void on__pushButtonOpen_clicked();
    void on__pushButtonLoadOld_clicked();
    void onProcessOfOpenFinished();
    void onProcessOfParsingFinished();

signals:
    void toDebug(QString, QString);

private:
    Ui::DatabaseWidget             *_ui;
    Database                       *_db;
    QFutureWatcher<ListAddress>     _futureWatcher;
    QFutureWatcher<void>            _futureWatcherParser;
    ListAddress                     _addrs;
    QString                         _csvFileName;
//    QString                         _dbFileName;

    void connectModelWithView(QSqlTableModel *model);
//    void parsingAddress(Address &a);

};

#endif // DATABASEWIDGET_H

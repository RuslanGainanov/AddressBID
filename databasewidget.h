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
    void open();
    void openExisting();
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
    QThread                         _thread;

    void connectModelWithView(QSqlTableModel *model);
//    void parsingAddress(Address &a);

};

#endif // DATABASEWIDGET_H

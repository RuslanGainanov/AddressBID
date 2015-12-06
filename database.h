#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "csvworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"
#include <QScopedPointer>
#include <QtSql>
#include <QSqlError>
#include <QSqlDatabase>


class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    ~Database();

signals:
    void headReaded(QStringList head);
    void headParsed(MapAddressElementPosition head);
    void rowReaded(int rowNumber);
    void rowParsed(int rowNumber);
    void readedRows(int count);
    void countRows(int count);
    void workingWithOpenBase();
    void baseOpened();

    void toDebug(QString);

public slots:
    void openBase(QString filename);
    void openOldBase(QString name);
    void removeAllDataInBase();
    void clear();

    void onReadRow(int rowNumber, QStringList row);
    void onParseRow(int rowNumber, Address addr);
    void onReadRows(int rows);
    void onCountRows(int count);
    void onFinishParser();
    void onFinishCsvWorker();

private:
    bool _connected;
    CsvWorker *_csvWorker;
//    QScopedPointer<CsvWorker> _csvWorker;
    Parser *_parser;
//    QScopedPointer<Parser> _parser;
    QThread *_thread;
    QSqlDatabase _db;
    QString _baseName;

    inline bool createConnection()
    {
        if(_connected)
            return true;
        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName(_baseName);
        _db.setUserName("user");
        _db.setHostName("rt");
        _db.setPassword("user");
        QString str ;
        if (!_db.open())
        {
            str += "Cannot open database:" + _db.lastError().text();
            qDebug()<< "error open ";
            emit toDebug(str);
            return false;
        }
        else
        {
            qDebug() << "success open";
            str += "Success open base:" + _baseName;
        }
        return true;
    }

};

#endif // DATABASE_H

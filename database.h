#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "csvworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"
#include <QtSql>

typedef QList< Address > ListAddress;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    ~Database();
    QSqlTableModel *getModel();

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
    void messageReady(QString);

public slots:
    void openBase(QString filename);
    void openOldBase();

    ListAddress search(QString sheetName, ListAddress addr);

    void setBaseName(QString name);
    void clear();

    void onReadRow(int rowNumber, QStringList row);
    void onParseRow(int rowNumber, Address addr);
    void onReadRows(int rows);
    void onCountRows(int count);
    void onFinishParser();
    void onFinishCsvWorker();

private:
    CsvWorker *_csvWorker;
//    Parser *_parser;
    QThread *_thread;
    QMap< AddressElements, QSet<QString> > _mapSet;
    QSqlDatabase _db;
    QString _baseName;
    bool _connected;
    int _countParsedRows;
    QSqlTableModel *_model;

    void openTableToModel();
    void createConnection();
    void createTable();
    void dropTable();
    void insertAddress(int row, const Address &a);
    void selectAddress(Address &a);
};

#endif // DATABASE_H

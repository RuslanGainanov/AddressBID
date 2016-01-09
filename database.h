#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include "address.h"
#include "defines.h"

const int CountTogetherInsertQuery=1000;

typedef QList< Address > ListAddress;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    ~Database();
    QSqlTableModel *getModel();
    void removeConnection();
    void createConnection();
    void dropTable();
    void createTable();
    void updateTableModel();
    void setBaseName(QString name);
    QString baseName();

signals:
    void headReaded(QStringList head);
    void headParsed(MapAddressElementPosition head);
    void rowReaded(int rowNumber);
    void rowParsed(int rowNumber);
    void readedRows(int count);
    void countRows(int count);
    void workingWithOpenBase();
    void baseOpened();

    void toDebug(QString,QString);

public slots:
    void openBase(QString filename);
    void removeBase(QString filename);

    ListAddress search(QString sheetName, ListAddress addr);

    void insertListAddressWithCheck(ListAddress &la);
    void insertAddressWithCheck(Address &a);
    void clear();

private:
    QThread *_thread;
    QSqlTableModel *_model;
//    QSqlDatabase _db;
    QString _baseName;
    bool _connected;
    ListAddress _addrs;
    QSet< quint64 > _bids;

    void openTableToModel();
    void selectAddress(Address &a);
};

#endif // DATABASE_H

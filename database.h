#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "csvworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"

typedef QVector<QString> VectorOfString;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    ~Database();

signals:
    void headReaded(QMap<QString, int> head);
    void rowReaded(int rowNumber);
    void readedRows(int count);
    void rowParsed(int rowNumber);
    void countRows(int count);

public slots:
    void openBase(QString filename);
    void clear();
    void onReadRow(int rowNumber, QStringList row);
    void onReadRows(int rows);
    void onCountRows(int count);
    void onParseRow(int rowNumber, Address addr);

private:
    QVector<VectorOfString> *_vect;
    CsvWorker *_csvWorker;
    Parser *_parser;
    QThread *_thread;
    QMap<QString, int> _mapHead; //Name column and position column
    int _countThreadParser;

};

#endif // DATABASE_H

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "csvworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"
#include <QScopedPointer>

typedef QVector<QString> VectorOfString;

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

public slots:
    void openBase(QString filename);
    void clear();

    void onReadRow(int rowNumber, QStringList row);
    void onParseRow(int rowNumber, Address addr);
    void onReadRows(int rows);
    void onCountRows(int count);
    void onFinishParser();
    void onFinishCsvWorker();

private:
    CsvWorker *_csvWorker;
//    QScopedPointer<CsvWorker> _csvWorker;
    Parser *_parser;
//    QScopedPointer<Parser> _parser;
    QThread *_thread;

};

#endif // DATABASE_H

#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include "defines.h"

class CsvWorker : public QObject
{
    Q_OBJECT
public:
    explicit CsvWorker(QObject *parent = 0);
    ~CsvWorker();
    void setFileName(QString fname);
    void setMaxCountRead(int maxCount);

signals:
    void rowReaded(int rowNumber, QStringList row);
    void rowsReaded(int count);
    void countRows(int count);
    void headReaded(QStringList head);
    void finished();

public slots:
    void process();

private:
    QString _filename;
    int _maxCount;

};

#endif // CSVWORKER_H

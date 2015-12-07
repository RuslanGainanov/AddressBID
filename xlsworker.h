#ifndef XLSWORKER_H
#define XLSWORKER_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include "defines.h"
#include <QAxBase>
#include <QAxObject>

class XlsWorker : public QObject
{
    Q_OBJECT
public:
    explicit XlsWorker(QObject *parent = 0);
    ~XlsWorker();
    void setFileName(QString fname);
    void setMaxCountRead(int maxCount);
    QString getListName(int numberList);

signals:
    void countLists(int count);
    void listNames(QStringList list);
    void rowReaded(int listNumber, int rowNumber, QStringList row);
    void rowsReaded(int listNumber, int count);
    void countRows(int listNumber, int count);
    void headReaded(int listNumber, QStringList head);
    void finished();

    void toDebug(QString);

public slots:
    void process();

private:
    QMultiMap<QString, QStringList> *_mapBase;
    QString _filename;
    QStringList _listNames;
    int _maxCount;

};

#endif // XLSWORKER_H

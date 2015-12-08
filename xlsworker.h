#ifndef XLSWORKER_H
#define XLSWORKER_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QAxBase>
#include <QAxObject>
#include "defines.h"

class XlsWorker : public QObject
{
    Q_OBJECT
public:
    explicit XlsWorker(QObject *parent = 0);
    ~XlsWorker();
    void setFileName(QString fname);
    void setMaxCountRead(int maxCount);

signals:
    void sheetsReaded(QMap<int, QString> sheets);
    void rowReaded(int sheet, int row, QStringList listRow);
    void rowsReaded(int sheet, int count);
    void countRows(int sheet, int count);
    void headReaded(int sheet, QStringList head);
    void errorOccured(QString nameObject, int code, QString errorDesc);
    void finished();

    void toDebug(QString);
    void toDebug(QString,QString);

public slots:
    void process();

private slots:
    void debugError(int,QString,QString,QString);

private:
    QString _filename;
    QMap<int, QString> _sheetNames;
    int _maxCount;

};

#endif // XLSWORKER_H

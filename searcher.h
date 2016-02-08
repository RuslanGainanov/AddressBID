#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>
#include <QtSql>
#include "address.h"
#include "defines.h"

class Searcher : public QObject
{
    Q_OBJECT
public:
    explicit Searcher(const QString &name = QString(),
                      QObject *parent = 0);
    ~Searcher();

    bool isCanceled();

signals:
    void addressFounded(QString sheet, int nRow, Address a);
    void addressNotFounded(QString sheet, int nRow, Address a);
//    void finished();
    void finished(QString sheet);
    void toDebug(QString object, QString mes);

public slots:
    void selectAddress(QString sheet, int nRow, Address a, bool findIt);
    void setCountRows(int rows);
    void cancel();

private:
    int _numberRowsForProcessing;
    int _receivedNumber;
    QString _sheet;
    bool _canceled;
    bool _isFinished;

};

#endif // SEARCHER_H

#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include "defines.h"
#include "address.h"

class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);
    ~Parser();

signals:
    void rowParsed(int rowNumber, Address addr);
    void finished();
    void headBaseReaded(QMap<QString, int> head);


public slots:
    void process();

    void setHeadBase(const QMap<QString, int> &head);
    void setHeadIn(const QMap<QString, int> &head);
    void setTypeOfRow(const TypeOfRow &type);
    void onReadRow(const int &rowNumber, const QStringList &row);
    void onReadHeadBase(QStringList head);

private:
    QMap<QString, int> _mapHeadBase; //Name column and position column
    QMap<QString, int> _mapHeadIn;
    QStringList _row;
    int _rowNumber;
    Address _addr;
    TypeOfRow _type;

    void parseBaseRow();
    void parseInRow();

public slots:

};

#endif // PARSER_H

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
    void headBaseParsed(MapAddressElementPosition head);


public slots:
    void process();

    void setTypeOfRow(const TypeOfRow &type);
    void onReadRow(const int &rowNumber, const QStringList &row);
    void onReadHeadBase(QStringList head);

private:
    MapAddressElementPosition _mapHeadBaseAddr;
    QStringList _row;
    int _rowNumber;
    Address _addr;
    TypeOfRow _type;

    void parseBaseRow();
    void parseInRow();

    void parseCity(QString &str, Address &a);
    void parseAdditional(QString &str, Address &a);

public slots:

};

#endif // PARSER_H

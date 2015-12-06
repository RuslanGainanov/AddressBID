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

    void parseBuild(QString &str, Address &a, int &offset);
    void parseKorp(QString &str, Address &a, int &offset);
    void parseDistrict(QString &str, Address &a, int &offset);
    void parseFSubject(QString &str, Address &a, int &offset);
    void parseCity(QString &str, Address &a, int &offset);
    void parseAdditional(QString &str, Address &a, int &offset);
    void parseStreet(QString &str, Address &a, int &offset); //and Ename
};

#endif // PARSER_H

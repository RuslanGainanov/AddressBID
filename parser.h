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
    void setTypeOfRow(const TypeOfRow &type);

signals:
    void rowParsed(int rowNumber, Address addr);
    void rowParsed(int sheet, int row, Address addr);
    void headBaseParsed(MapAddressElementPosition head);
    void headInputParsed(int sheet, MapAddressElementPosition head);

    void finished();

public slots:
    void process();

    void onReadRow(const int &rowNumber, const QStringList &row);

    //inputFile only //TODO make individual class
    void onReadRow(const int &sheet, const int &rowNumber, const QStringList &row);
    void onReadHeadBase(QStringList head);
    void onReadHeadInput(int sheet, QStringList head);

private:
    MapAddressElementPosition _mapHeadBase;
    QMap<int, MapAddressElementPosition> _mapHeadInput;
    QStringList _row;
    int _rowNumber;
    int _sheet;
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

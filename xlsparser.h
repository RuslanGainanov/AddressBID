#ifndef XLSPARSER_H
#define XLSPARSER_H

#include <QObject>
#include "defines.h"
#include "address.h"

class XlsParser : public QObject
{
    Q_OBJECT
public:
    explicit XlsParser(QObject *parent = 0);
    ~XlsParser();

signals:
    void finished();
    void rowParsed(int sheet, int row, Address addr);
    void headParsed(int sheet, MapAddressElementPosition head);

public slots:
    void process();
    void onReadHead(int sheet,
                    QStringList head);
    void onReadRow(const int &sheet,
                   const int &rowNumber,
                   const QStringList &row);

private:
    QMap<int, MapAddressElementPosition> _mapHead;

    bool parseObject(QString &str, QString &result, QString regPattern, int regCap);

//    void parseBuild(QString &str, Address &a, int &offset);
//    void parseKorp(QString &str, Address &a, int &offset);
//    void parseDistrict(QString &str, Address &a, int &offset);
//    void parseFSubject(QString &str, Address &a, int &offset);
//    void parseCity(QString &str, Address &a, int &offset);
//    void parseAdditional(QString &str, Address &a, int &offset);
//    void parseStreet(QString &str, Address &a, int &offset); //and Ename

};

#endif // XLSPARSER_H

#include "xlsparser.h"

XlsParser::XlsParser(QObject *parent) :
    QObject(parent)
{
    qDebug() << "XlsParser constructor" << this->thread()->currentThreadId();
}

XlsParser::~XlsParser()
{
    qDebug() << "XlsParser destructor" << this->thread()->currentThreadId();
}

void XlsParser::process()
{
    qDebug() << "XlsParser process" << this->thread()->currentThreadId();
    emit finished();
}

void XlsParser::onReadHead(int sheet,
                           QStringList head)
{
//    qDebug() << "XlsParser onReadHead"
//             << sheet << head
//             << this->thread()->currentThreadId();
    QList< AddressElements > listAddressElements = MapColumnNames.keys();
    for(int i=0; i<listAddressElements.size(); i++)
    {
        QString colname=MapColumnNames[ listAddressElements.at(i) ];
        if(!head.contains(colname))
            head.append(colname);
    }
    QMap<AddressElements, QString>::const_iterator it = MapColumnNames.begin();
    while(it!=MapColumnNames.end())
    {
        _mapHead[sheet].insert(it.key(), head.indexOf(it.value()));
        it++;
    }
    emit headParsed(sheet, _mapHead[sheet]);
}

void XlsParser::onReadRow(const int &sheet,
                          const int &rowNumber,
                          const QStringList &row)
{
//    qDebug() << "XlsParser onReadRow" << rowNumber
//             /*<< row */<< this->thread()->currentThreadId() ;

    Address a;
    a.setRawAddress(row);
    //парсинг строки начат


    //парсинг строки окончен

    emit rowParsed(sheet, rowNumber, a); //парсинг строки окончен
    emit finished();
}


bool XlsParser::parseObject(QString &str, QString &result,
                            QString regPattern, int regCap)
{
    if(str.isEmpty())
        return false;
    QRegExp rx(regPattern);
    int pos=rx.indexIn(str);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        result = rx.cap(regCap).trimmed();
        return true;
    }
    else
        return false;
}

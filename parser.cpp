#include "parser.h"

Parser::Parser(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Parser constructor" << this->thread()->currentThreadId();
}


Parser::~Parser()
{
    qDebug() << "Parser destructor" << this->thread()->currentThreadId();;
}


void Parser::process()
{
//    qDebug() << "Parser process" << this->thread()->currentThreadId();
    switch (_type) {
    case BASE:
        parseBaseRow();
        break;
    case INPUT:
        parseInRow();
        break;
    default:
        break;
    }
}

void Parser::setHeadBase(const QMap<QString, int> &head)
{
    qDebug() << "Parser setHeadBase" << this->thread()->currentThreadId();
    _mapHeadBase=head;
}

void Parser::setHeadIn(const QMap<QString, int> &head)
{
    _mapHeadIn=head;
}

void Parser::setTypeOfRow(const TypeOfRow &type)
{
    _type=type;
}

void Parser::onReadRow(const int &rowNumber, const QStringList &row)
{
    qDebug() << "Parser onReadRow" << this->thread()->currentThreadId() << rowNumber;
    _rowNumber=rowNumber;
    _row=row;
}

void Parser::onReadHeadBase(QStringList head)
{
    qDebug() << "Parser onReadHeadBase" << this->thread()->currentThreadId();
    for(int i =0; i<ListAddressElements.size(); i++)
    {
        QString colname=MapColumnNames[ ListAddressElements.at(i) ];
        if(!head.contains(colname))
            head.append(colname);
    }
    for(int i=0; i<head.size(); i++)
    {
        _mapHeadBase.insert(head.at(i), i);
    }
    emit headBaseReaded(_mapHeadBase);
}

void Parser::parseBaseRow()
{
    qDebug() << "Parser parseBaseRow" << this->thread()->currentThreadId() << _rowNumber;
    Address a;
    a.setRawAddress(_row);
    //парсинг строки начат
    //...
    //парсинг строки окончен!

    emit rowParsed(_rowNumber, a);
    emit finished();
}

void Parser::parseInRow()
{
    qDebug() << "Parser parseInRow" << this->thread()->currentThreadId();
    emit finished();
}

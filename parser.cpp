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
    case BASE_TYPE:
        parseBaseRow();
        break;
    case INPUT_TYPE:
        parseInRow();
        break;
    default:
        break;
    }
    emit finished();
}

void Parser::setTypeOfRow(const TypeOfRow &type)
{
    _type=type;
}

void Parser::onReadRow(const int &sheet,
                       const int &rowNumber,
                       const QStringList &row)
{
    _rowNumber=rowNumber;
    _row=row;
    _sheet=sheet;
    parseInRow();
    emit finished();
}

void Parser::onReadRow(const int &rowNumber, const QStringList &row)
{
//    qDebug() << "Parser onReadRow" << this->thread()->currentThreadId() << rowNumber;
    _rowNumber=rowNumber;
    _row=row;
    switch (_type) {
    case BASE_TYPE:
        parseBaseRow();
        break;
    case INPUT_TYPE:
        parseInRow();
        break;
    default:
        break;
    }
    emit finished();
}

void Parser::onReadHeadInput(int sheet, QStringList head)
{
    qDebug() << "Parser onReadHeadInput"
             << sheet << head
             << this->thread()->currentThreadId();

}

void Parser::onReadHeadBase(QStringList head)
{
    qDebug() << "Parser onReadHeadBase" << this->thread()->currentThreadId();
    for(int i=0; i<ListAddressElements.size(); i++)
    {
        QString colname=MapColumnNames[ ListAddressElements.at(i) ];
        if(!head.contains(colname))
            head.append(colname);
    }
    QMap<AddressElements, QString>::const_iterator it = MapColumnNames.begin();
    while(it!=MapColumnNames.end())
    {
        _mapHeadBase.insert(it.key(), head.indexOf(it.value()));
        it++;
    }
    emit headBaseParsed(_mapHeadBase);
}

void Parser::parseAdditional(QString &str, Address &a, int &offset)
{
    Q_UNUSED(offset);
    if(str.isEmpty())
        return;
    QRegExp rx("(?:^|\\W+)(село|поселок|пос|деревня|[спд]"
               "|ж/д_ст|ж/д_рзд|ж/д_пост|п/ст|массив|рп|нп|снт|пгт|сдт|"
               "тер|ст|промзона|метро|мост)\\W+([\\w\\d\\s-]+)[.,;()]");
    int pos=rx.indexIn(str);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        QString add(rx.cap(2).trimmed());
        if(a.getAdditional().isEmpty())
        {
            a.setAdditional(add);//добавляем найденное имя
        }
        else
        {
            QString currAdd = a.getAdditional();
            a.setAdditional(currAdd+add);
        }
    }
}

void Parser::parseStreet(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("(?:^|[.,;()])([\\w\\d\\s-]+)(ул|улица|пр-т|пр|проспект|пер|переулок|"
               "проезд|лин|линия|"
               "наб|набережная|парк|ш|шоссе|сад|пл|площадь|аллея|ал|кв-л|квартал|"
               "дор|канал|"
               "б-р|бульвар|х)\\W{0,}(?:[.,;()]|$)");
    int pos=rx.indexIn(str,offset);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setStreet(rx.cap(1).trimmed());
        QString e=rx.cap(2).trimmed();
        if(e=="ул"||
                e=="улица")
            a.setEname("ул");
        else if(e=="пр"||
                e=="пр-т"||
                e=="проспект")
            a.setEname("пр");
//        else if ... //TODO
        offset = pos+rx.cap(0).size();
    }
}


void Parser::parseDistrict(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("(?:^|[.,;()])([\\w\\d\\s-]+)(р-н)\\W{0,}(?:[.,;()]|$)");
    int pos=rx.indexIn(str,offset);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setDistrict(rx.cap(1).trimmed());
        offset = pos;
    }
}

void Parser::parseFSubject(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("(?:^|[.,;()])([\\w\\d\\s-]+)(респ|Респ|республика|обл|область|край|АО)\\W{0,}(?:[.,;()]|$)");
    int pos=rx.indexIn(str);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setFsubj(rx.cap(1).trimmed());
        offset = pos;
    }
}


void Parser::parseCity(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("(?:^|\\W+)(город|гор|г)\\W+([\\w\\s-]+)\\W{0,}(?:[.,;()]|$)");
    int pos=rx.indexIn(str,offset);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setCity(rx.cap(2).trimmed());
        offset = pos;
    }
}

void Parser::parseBuild(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("((?:^[,. ;]{0,}|\\s+)|д.|д|Д|Д.|дом|нетр|ая|а/я)\\s{0,}(\\d+)(?:\\D+|$)");
    int pos=rx.indexIn(str,offset);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setBuild(rx.cap(2).trimmed());
        offset = pos;
    }
}

void Parser::parseKorp(QString &str, Address &a, int &offset)
{
    if(str.isEmpty())
        return;
    QRegExp rx("((?:^[,. ;]{0,}|\\s+)|к.|к|К|К.|корп.|корпус|корп|лит.|литера|лит)\\s{0,}(\\d+|\\w+)(?:[ ,.;()]|$)");
    int pos=rx.indexIn(str,offset);
    if(pos!=-1)
    {
        str.remove(rx.cap(0)); //удаляем все что нашли
        a.setCity(rx.cap(2).trimmed());
        offset = pos;
    }
}

void Parser::parseBaseRow()
{
//    qDebug() << "Parser parseBaseRow"
//             << this->thread()->currentThreadId()
//             << _rowNumber
//             << _row.join('|');
    Address a;
    a.setRawAddress(_row);
    //парсинг строки начат

    QString additional;
    int offset=0;
    for(int i=0; i<_row.size(); i++)
    {
        QString str=_row[i];
//        if(i == _mapHeadBaseAddr[STREET]
//                && !str.contains("г. Санкт-Петербург", Qt::CaseInsensitive))
//            break;

        //удаляем боковые символы ""
        str.remove("\"");
        str=str.trimmed();

        if(i==_mapHeadBase[STREET_ID])
        {
            a.setStreetId(str.toULongLong());
            continue;
        }
        if(i==_mapHeadBase[BUILD_ID])
        {
            a.setBuildId(str.toULongLong());
            continue;
        }

        //работаем с STR
        if(i==_mapHeadBase[STREET])
        {
//            str = str.toLower();
            parseFSubject(str, a, offset);
            parseDistrict(str, a, offset);
            parseCity(str, a, offset);
            parseAdditional(str, a, offset);
            parseStreet(str, a, offset);

            //работа со скобками
            int n1=str.indexOf('(');
            if (n1>0 && (str.indexOf(')',n1)>0))
            {
                additional+=a.getAdditional();
                int n2=str.indexOf(')', n1);
                int n3=n2-n1;
                additional+=str.mid(n1+1, n3-1);
                str.remove(n1, n3+1);
                a.setAdditional(additional);
            }
            str = str.trimmed();
        } // конец работы с STR

        //работаем с B
        if(i==_mapHeadBase[BUILD])
        {
            parseBuild(str, a, offset);
            continue;
        }//end work with B

        //работаем с K
        if(i==_mapHeadBase[KORP])
        {
            parseKorp(str, a, offset);
            continue;
        }
    }
    //парсинг строки окончен!

    emit rowParsed(_rowNumber, a);
}

void Parser::parseInRow()
{
    qDebug() << "Parser parseInRow" << _rowNumber
             /*<< _row */<< this->thread()->currentThreadId() ;

    Address a;
    a.setRawAddress(_row);
    //парсинг строки начат

    emit rowParsed(_sheet, _rowNumber, a); //парсинг строки окончен

}

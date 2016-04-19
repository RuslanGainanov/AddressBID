#include "xlsparser.h"

XlsParser::XlsParser(QObject *parent) :
    QObject(parent)
//  , _error(false)
{
//    qDebug() << "XlsParser constructor" << this->thread()->currentThreadId();
}

XlsParser::~XlsParser()
{
//    qDebug() << "XlsParser destructor" << this->thread()->currentThreadId();
}

void XlsParser::onReadHead(const QString sheet,
                           MapAddressElementPosition head)
{
//    qDebug() << "XlsParser onReadHead"
//             << sheet << head.values()
//             << this->thread()->currentThreadId();
    _mapHead.insert(sheet, head);
}

void XlsParser::onReadRow(const QString &sheet,
                          const int &rowNumber,
                          const QStringList &row)
{
//    qDebug() << "XlsParser onReadRow" << sheet << rowNumber
//             /*<< row */<< this->thread()->currentThreadId() ;

    Address a;
    QString str;
    QString addrString;

    if(_mapHead[sheet].contains(FSUBJ))
    {
        str = row.at(_mapHead[sheet].value(FSUBJ));
        str.remove(',');
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(DISTRICT))
    {
        str = row.at(_mapHead[sheet].value(DISTRICT));
        str.remove(',');
        if(!str.contains(QRegExp("\\b(?:район|р-н)\\b",Qt::CaseInsensitive)))
            str.append("р-н");
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(CITY1))
    {
        str = row.at(_mapHead[sheet].value(CITY1));
        str.remove(',');
        if(!str.contains(QRegExp("[\\W]",Qt::CaseInsensitive)))
            str=QString("г. ")+str;
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(STREET))
    {
        str = row.at(_mapHead[sheet].value(STREET));
        if(!str.contains(QRegExp("\\b(?:улица|ул\\.*|шоссе|ш\\.*|аллея|ал\\.*|бульвар|б-р|линия|лин\\.*|набережная|наб\\.*|парк|переулок|пер\\.*|площадь|пл\\.*|проспект|пр-кт|пр\\.*|сад|сквер|строение|стр\\.*|участок|уч-к|квартал|берег|кв-л|километр|км|кольцо|проезд|переезд|въезд|заезд|дорога|дор\\.*|платформа|платф\\.*|площадка|пл-ка\\.*|полустанок|полуст\\.*|проулок|просека|тракт|тупик|туп\\.*|просёлок)\\b",Qt::CaseInsensitive)))
            str=QString("ул. ")+str;
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(BUILD))
    {
        str = row.at(_mapHead[sheet].value(BUILD));
        str.remove(',');
        if(!str.contains(QRegExp("\\b(?:д\\.*|дом\\.*|ая\\.*|а\\\\я\\.*|а/я\\.*)\\b",Qt::CaseInsensitive)))
            str=QString("д. ")+str;
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(KORP))
    {
        str = row.at(_mapHead[sheet].value(KORP));
        str.remove(',');
        if(!str.contains(QRegExp("\\b(?:к\\.*|корп\\.*)\\b",Qt::CaseInsensitive)))
            str=QString("к. ")+str;
        addrString.append(str);
        addrString.append(", ");
    }
    if(_mapHead[sheet].contains(LITERA))
    {
        str = row.at(_mapHead[sheet].value(LITERA));
        str.remove(',');
        if(!str.contains(QRegExp("\\b(?:л\\.*|лит\\.*|литера\\.*)\\b",Qt::CaseInsensitive)))
            str=QString("лит. ")+str;
        addrString.append(str);
        addrString.append(" ");
    }

    if(_mapHead[sheet].contains(ADDITIONAL))
    {
        str = row.at(_mapHead[sheet].value(ADDITIONAL));
        addrString.append("(");
        addrString.append(str);
        addrString.append(")");
    }

    parseAddress(addrString, a);

//    qDebug().noquote() << " XlsParser::" << str << a.toString(PARSED);
//    assert(0);
    emit rowParsed(sheet, rowNumber, a); //парсинг строки окончен
}


void XlsParser::parseAddress(QString &str, Address &a)
{
    QStringList resList;
    QString ptrn;
    str = str.simplified();
    str = str.toLower();

    //работаем с федеральным субъектом
    ptrn="((?:^|[.,;()]\\s*)(?:(?:([\\w\\d\\s-]+)\\s+(республика|респ\\.*|область|обл\\.*|край|ао|aобл\\.*))|(?:(республика|респ\\.*|ао|aобл\\.*)\\s+([\\w\\d\\s-]+))))(?:[.,;()]|$)";
    if(parseObject(str,
                   resList,
                   ptrn))
    {
        str.remove(resList.at(1));
        if(!resList.at(2).isEmpty())
            a.setFsubj(resList.at(2));
        else if(!resList.at(5).isEmpty())
            a.setFsubj(resList.at(5));
        if(!resList.at(3).isEmpty())
            a.setTypeOfFSubj(resList.at(3));
        else if(!resList.at(4).isEmpty())
            a.setTypeOfFSubj(resList.at(4));
    }
    //с районом
    ptrn = "((?:^|[.,;()]\\s*)"
           "(?:(?:([\\w\\d\\s-]+)\\s+(?:район|р-н))|"
           "(?:(?:район|р-н)\\s+([\\w\\d\\s-]+))))"
           "(?:[.,;()]|$)";
    if(parseObject(str,
                   resList,
                   ptrn))
    {
        str.remove(resList.at(1));
        if(!resList.at(2).isEmpty())
            a.setDistrict(resList.at(2));
        else if(!resList.at(3).isEmpty())
            a.setDistrict(resList.at(3));
    }
    //с городом
    //учитывает пропуск вначале и дом от деревни (по следующей цифре за "д.")
    ptrn="\\b(село|деревня|дер\\.*|поселок|пос\\.*|станция|ст\\.*|(?:[сдп]\\.*(?!\\s*\\d+))|город|гор\\.*|г\\.*|ж\\/д_рзд\\.|рп\\.|ж\\/д_ст\\.|высел\\.|х\\.*|казарма\\.*|м\\.|жилрайон\\.*|нп\\.*|снт\\.*|дп\\.*|снт\\.*|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|тер\\.*|массив\\.*)(\\s+[\\w\\d\\s.-]+)(?:([.,;]|$)|[()])";
    if(parseObject(str,
                   resList,
                   ptrn))
    {
        str.remove(resList.at(1)+resList.at(2)+resList.at(3));
        a.setTypeOfCity1(resList.at(1));
        a.setCity1(resList.at(2));
    }

    //учитывает пропуск вначале и дом от деревни (по следующей цифре за "д.")
    ptrn="\\b(село|деревня|дер\\.*|поселок|пос\\.*|станция|ст\\.*|(?:[сдп]\\.*(?!\\s*\\d+))|город|гор\\.*|г\\.*|ж\\/д_рзд\\.|рп\\.|ж\\/д_ст\\.|высел\\.|х\\.*|казарма\\.*|м\\.|жилрайон\\.*|нп\\.*|снт\\.*|дп\\.*|снт\\.*|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|тер\\.*|массив\\.*)(\\s+[\\w\\d\\s.-]+)(?:([.,;]|$)|[()])";
    if(parseObject(str,
                   resList,
                   ptrn))
    {

        str.remove(resList.at(1)+resList.at(2)+resList.at(3));
        a.setTypeOfCity2(resList.at(1));
        a.setCity2(resList.at(2));
    }

    //с улицей, пр-том, и пр.
    ptrn = "((?:[.,;()]*\\s*)"
           "(?:(?:(улица|ул\\.*|шоссе|ш\\.*|аллея|ал\\.*|бульвар|б-р|линия|лин\\.*|набережная|наб\\.*|парк|переулок|пер\\.*|площадь|пл\\.*|проспект|пр-кт|пр\\.*|сад|сквер|строение|стр\\.*|участок|уч-к|квартал|берег|кв-л|километр|км|кольцо|проезд|переезд|въезд|заезд|дорога|дор\\.*|платформа|платф\\.*|площадка|пл-ка\\.*|полустанок|полуст\\.*|проулок|просека|тракт|тупик|туп\\.*|просёлок)\\s+([\\w\\d\\s-.]+))|"
           "(?:([\\w\\d\\s-.]+)\\s+(улица|ул\\.*|шоссе|ш\\.*|аллея|ал\\.*|бульвар|б-р|линия|лин\\.*|набережная|наб\\.*|парк|переулок|пер\\.*|площадь|пл\\.*|проспект|пр-кт|пр\\.*|сад|сквер|строение|стр\\.*|участок|уч-к|квартал|берег|кв-л|километр|км|кольцо|проезд|переезд|въезд|заезд|дорога|дор\\.*|платформа|платф\\.*|площадка|пл-ка\\.*|полустанок|полуст\\.*|проулок|просека|тракт|тупик|туп\\.*|просёлок)))\\s*)"
           "(?:([,;]|$)|[()])";
    if(parseObject(str,
                   resList,
                   ptrn))
    {
        str.remove(resList.at(1));
        if(!resList.at(2).isEmpty())
            a.setTypeOfStreet(resList.at(2));
        else if(!resList.at(5).isEmpty())
            a.setTypeOfStreet(resList.at(5));
        if(!resList.at(3).isEmpty())
            a.setStreet(resList.at(3));
        else if(!resList.at(4).isEmpty())
            a.setStreet(resList.at(4));
    }

    //дом, корпус и литера
    ptrn = "(?:[.,;]*\\s*)(д\\.*|дом\\.*|ая\\.*|а\\\\я\\.*|а/я\\.*)\\s*([0-9Х][-\\\\/0-9]*)([а-я]*)\\s*,*\\s*(?:(к\\.*|корп\\.*)\\s*(\\d*)-*(\\w*))*\\s*,*\\s*(?:(л\\.*|лит\\.*|литера\\.*)\\s*(\\d*)-*(\\w*))*\\s*(?:(?:кв\\.*)|([а-я]*))";
    if(parseObject(str,
                   resList,
                   ptrn))
    {
        str.remove(resList.at(0));
        a.setBuild(resList.at(2));
        QString k;
        if(!resList.at(5).isEmpty())
            k=resList.at(5);
        else
            k=resList.at(8);
        QString l;
        l=resList.at(3)+resList.at(10)+resList.at(6)+
                resList.at(10)+resList.at(9);
        a.setKorp(k);
        a.setLitera(l);
    }
    //работа со скобками
    int n1=str.indexOf('(');
    QString additional;
    while(n1>=0)
    {
        int n2=str.indexOf(')', n1);
        if(n2>=0)
        {
            int n3=n2-n1;
            additional+=str.mid(n1+1, n3-1)+" ";
            str.remove(n1, n3+1);
        }
        else
            str.remove(n1, 1);
        n1=str.indexOf('(');
    }
    str.remove('(').remove(')');
    additional.remove('(').remove(')');
    if(!additional.isEmpty())
        a.setAdditional(additional);
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

bool XlsParser::parseObject(QString &str, QStringList &result,
                            QString regPattern)
{
    if(str.isEmpty())
        return false;
    QRegExp rx(regPattern);
    int pos=rx.indexIn(str);
    if(pos!=-1)
    {
//        str.remove(rx.cap(0)); //удаляем все что нашли
        result = rx.capturedTexts();
        return true;
    }
    else
        return false;
}

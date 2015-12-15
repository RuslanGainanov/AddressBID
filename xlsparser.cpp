#include "xlsparser.h"

XlsParser::XlsParser(QObject *parent) :
    QObject(parent)
  , _data(nullptr)
  , _error(false)
  , _isOneColumn(false)
{
    qDebug() << "XlsParser constructor" << this->thread()->currentThreadId();
}

XlsParser::~XlsParser()
{
    qDebug() << "XlsParser destructor" << this->thread()->currentThreadId();
}

void XlsParser::setTableOfData(QMap<QString, TableModel *> *data)
{
    _data=data;
}

void XlsParser::process()
{
//    qDebug() << "XlsParser process" << this->thread()->currentThreadId();
//    QStringList sheets = _data->keys();
//    foreach (QString sheet, sheets) {
//        TableModel *tm=(*_data)[sheet];
//        QStringList head = tm->getHeader();
//        qDebug() << "XlsParser getHeader" << head;
//        onReadHead(sheet, head);
//        qDebug() << "XlsParser getHeader after" << head;
//        qDebug() << "XlsParser getHeader mapHead" << _mapHead.value(sheet).values();
//        if(!_error)
//            for(int row=0; row<tm->rowCount(); row++)
//            {
//                Address a = onReadRow(sheet, row, tm->getRow(row));//парсим строку

//                int nCol=0;
//                nCol = _mapPHead.value(sheet).value(FSUBJ);//получаем номер столбца
//                tm->setData(tm->index(row, nCol), a.getFsubj());//заносим в ячейку распарсенный элемент

//                nCol = _mapPHead.value(sheet).value(DISTRICT);
//                tm->setData(tm->index(row, nCol), a.getDistrict());

//                nCol = _mapPHead.value(sheet).value(CITY);
//                tm->setData(tm->index(row, nCol), a.getCity());

//                nCol = _mapPHead.value(sheet).value(ADDITIONAL);
//                tm->setData(tm->index(row, nCol), a.getAdditional());

//                nCol = _mapPHead.value(sheet).value(STREET);
//                tm->setData(tm->index(row, nCol), a.getStreet());

//                nCol = _mapPHead.value(sheet).value(BUILD);
//                tm->setData(tm->index(row, nCol), a.getBuild());

//                nCol = _mapPHead.value(sheet).value(KORP);
//                tm->setData(tm->index(row, nCol), a.getKorp());
//            }
//        emit sheetParsed(sheet);
//    }
//    emit finished();
}

void XlsParser::onReadHead(const QString sheet,
                           MapAddressElementPosition head)
{
//    qDebug() << "XlsParser onReadHead"
//             << sheet << head.values()
//             << this->thread()->currentThreadId();
    _mapHead.insert(sheet, head);
}

void XlsParser::onIsOneColumn(bool b)
{
    _isOneColumn=b;
}

void XlsParser::onReadRow(const QString &sheet,
                          const int &rowNumber,
                          const QStringList &row)
{
//    qDebug() << "XlsParser onReadRow" << sheet << rowNumber
//             /*<< row */<< this->thread()->currentThreadId() ;

    Address a;
//    a.setRawAddress(row);
    //парсинг строки начат
    QString str = row.at(_mapHead[sheet].value(STREET));
    QStringList resList;
    QString ptrn;
    str = str.trimmed();
    str = str.toLower();

    //работаем с CITY
    if(_mapHead[sheet].contains(CITY))
    {
        str = row.at(_mapHead[sheet].value(CITY));
        str = str.trimmed();
        str = str.toLower();

        //с городом
        ptrn = "((?:(?:^)|(?:[.,;()])\\s*)"
               "(?:(?:(?:город|гор\\.*|г\\.*)\\s+([\\w\\d\\s-]+))|"
               "(?:([\\w\\d\\s-]+)\\s+(?:город|гор\\.*|г\\.*)))\\s*)"
               "(?:(?:[,;()])|(?:$))";
        if(parseObject(str,
                       resList,
                       ptrn))
        {
            str.remove(resList.at(1));
            if(!resList.at(2).isEmpty())
                a.setCity(resList.at(2));
            else if(!resList.at(3).isEmpty())
                a.setCity(resList.at(3));
        }
    }

    //работаем с STREET
    if(_mapHead[sheet].contains(STREET))
    {
        str = row.at(_mapHead[sheet].value(STREET));
        str = str.trimmed();
        str = str.toLower();

        //работаем с федеральным субъектом
        ptrn = "((?:^|[.,;()]\\s*)"
               "(?:(?:([\\w\\d\\s-]+)\\s+(?:республика|респ\\.*|область|обл\\.*|край|ао|aобл\\.*))|"
               "(?:(?республика|респ\\.*|ао|aобл\\.*)\\s+([\\w\\d\\s-]+))))"
               "(?:[.,;()]|$)";
        if(parseObject(str,
                       resList,
                       ptrn))
        {
            str.remove(resList.at(1));
            if(!resList.at(2).isEmpty())
                a.setFsubj(resList.at(2));
            else if(!resList.at(3).isEmpty())
                a.setFsubj(resList.at(3));
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
        ptrn = "((?:^|[.,;()]\\s*)"
               "(?:(?:(?:город|гор\\.*|г\\.*)\\s+([\\w\\d\\s-]+))|"
               "(?:([\\w\\d\\s-]+)\\s+(?:город|гор\\.*|г\\.*))))"
               "(?:[.,;()]|$)";
        if(parseObject(str,
                       resList,
                       ptrn))
        {
            str.remove(resList.at(1));
            if(!resList.at(2).isEmpty())
                a.setCity(resList.at(2));
            else if(!resList.at(3).isEmpty())
                a.setCity(resList.at(3));
        }
        //с селом, деревней поселком
        ptrn = "((?:^|[.,;()]\\s*)"
               "(?:(?:(?:село|деревня|дер\\.*|поселок|пос\\.*|станция|ст\\.*|[сдп]\\.*)\\s+([\\w\\d\\s-]+))|"
               "(?:([\\w\\d\\s-]+)\\s+(?:село|деревня|дер\\.*|поселок|пос\\.*|станция|ст\\.*|[сдп]\\.*))))"
               "(?:[.,;()]|$)";
        if(parseObject(str,
                       resList,
                       ptrn))
        {
            str.remove(resList.at(1));
            if(!resList.at(2).isEmpty())
                a.setAdditional(resList.at(2));
            else if(!resList.at(3).isEmpty())
                a.setAdditional(resList.at(3));
        }
        //с улицей, пр-том, и пр.
        ptrn = "((?:(?:^)|(?:[.,;()])\\s*)"
               "(?:(?:(улица|ул\\.*|шоссе|ш\\.*|аллея|ал\\.*|бульвар|б-р|линия|лин\\.*|набережная|наб\\.*|парк|переулок|пер\\.*|площадь|пл\\.*|проспект|пр-кт|пр\\.*|сад|сквер|строение|стр\\.*|участок|уч-к|квартал|берег|кв-л|километр|км|кольцо|проезд|переезд|въезд|заезд|дорога|дор\\.*|платформа|платф\\.*|площадка|пл-ка\\.*|полустанок|полуст\\.*|проулок|просека|тракт|тупик|туп\\.*|просёлок)\\s+([\\w\\d\\s-.]+))|"
               "(?:([\\w\\d\\s-.]+)\\s+(улица|ул\\.*|шоссе|ш\\.*|аллея|ал\\.*|бульвар|б-р|линия|лин\\.*|набережная|наб\\.*|парк|переулок|пер\\.*|площадь|пл\\.*|проспект|пр-кт|пр\\.*|сад|сквер|строение|стр\\.*|участок|уч-к|квартал|берег|кв-л|километр|км|кольцо|проезд|переезд|въезд|заезд|дорога|дор\\.*|платформа|платф\\.*|площадка|пл-ка\\.*|полустанок|полуст\\.*|проулок|просека|тракт|тупик|туп\\.*|просёлок)))\\s*)"
               "(?:(?:[,;()])|(?:$))";
        if(parseObject(str,
                       resList,
                       ptrn))
        {
            str.remove(resList.at(1));
            if(!resList.at(2).isEmpty())
                a.setEname(resList.at(2));
            else if(!resList.at(5).isEmpty())
                a.setEname(resList.at(5));
            if(!resList.at(3).isEmpty())
                a.setStreet(resList.at(3));
            else if(!resList.at(4).isEmpty())
                a.setStreet(resList.at(4));
        }
        //работа со скобками
        int n1=str.indexOf('(');
        if (n1>0 && (str.indexOf(')',n1)>0))
        {
            QString additional=a.getAdditional()+" ";
            int n2=str.indexOf(')', n1);
            int n3=n2-n1;
            additional+=str.mid(n1+1, n3-1);
            str.remove(n1, n3+1);
            a.setAdditional(additional);
        }

        //если в этом же столбце находится номер дома и корпус
        if(_isOneColumn)
        {
            a.setBuild("B1");
            a.setKorp("K1");
        }
    } // конец работы с STR

    //если номер дома и корпус находятся в другом столбце
    if(!_isOneColumn)
    {
        //работаем с B
        if(_mapHead[sheet].contains(BUILD))
        {
            QString str = row.at(_mapHead[sheet].value(BUILD));
            a.setBuild(str);
        }//end work with B

        //работаем с K
        if(_mapHead[sheet].contains(KORP))
        {
            QString str = row.at(_mapHead[sheet].value(KORP));
            a.setKorp(str);
        }//end work with K
    }
    //парсинг строки окончен

    emit rowParsed(sheet, rowNumber, a); //парсинг строки окончен
//    return a;
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

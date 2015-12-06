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
    emit finished();
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
    emit finished();
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
        _mapHeadBaseAddr.insert(it.key(), head.indexOf(it.value()));
        it++;
    }
    emit headBaseParsed(_mapHeadBaseAddr);
}

void Parser::parseAdditional(QString &str, Address &a)
{
//    QRegExp rx("(?:^|\W+)(село|поселок|пос|деревня|[спд]|ж/д_ст|ж/д_рзд|ж/д_пост|п/ст|массив|рп|снт|пгт|сдт|тер.сот|ст)\W+(\w+|(?:\w+-(?:\w+|\d+)|\w+\s+\d+))");
    QRegExp rx("(?:^|\\W+)(село|поселок|пос|деревня|[спд]|ж/д_ст|ж/д_рзд|ж/д_пост|п/ст|массив|рп|снт|пгт|сдт|тер.сот|ст)\\W+([\\w\\d\\s-]+)[.,;()]");
    if(rx.indexIn(str)!=-1)
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

void Parser::parseCity(QString &str, Address &a)
{
    QRegExp rx("(?:^|\\W+)(город|гор|г)\\W+([\\w\\d\\s-]+)[.,;()]");
    if(rx.indexIn(str)!=-1)
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
void Parser::parseBaseRow()
{
    qDebug() << "Parser parseBaseRow" << this->thread()->currentThreadId() << _rowNumber;
    Address a;
    a.setRawAddress(_row);
    //парсинг строки начат

    QString additional;
    QString ename;
    for(int i=0; i<_row.size(); i++)
    {
        QString str=_row[i];
        if(i == _mapHeadBaseAddr[STREET]
                && str.contains("г. Санкт-Петербург", Qt::CaseInsensitive))
            break;

        //удаляем боковые символы
        str.remove("\"");
        str=str.trimmed();

        if(i==_mapHeadBaseAddr[STREET_ID])
        {
            a.setStreetId(str.toULongLong());
            continue;
        }
        if(i==_mapHeadBaseAddr[BUILD_ID])
        {
            a.setBuildId(str.toULongLong());
            continue;
        }

        //работаем с STR
        if(i==_mapHeadBaseAddr[STREET])
        {
            str = str.toLower();
            //работаем с именами элементов (приведение их к одному формату)
            //(ул., пр., наб., ш., б., пер. и пр.)
            if (str.contains("ул.,"))
            {
              str.remove("ул.,");
              ename.append("ул");
            }
            if (str.contains("пр-кт.,"))
            {
              str.remove("пр-кт.,");
              ename.append("пр-кт");
            }
            if (str.contains("пер.,"))
            {
              str.remove("пер.,");
              ename.append("пер");
            }
            if (str.contains("проезд.,"))
            {
              str.remove("проезд.,");
              ename.append("проезд");
            }
            if (str.contains("линия.,"))
            {
              str.remove("линия.,");
              ename.append("линия");
            }
            if (str.contains("наб.,"))
            {
              str.remove("наб.,");
              ename.append("наб");
            }
            if (str.contains("парк.,"))
            {
              str.remove("парк.,");
              ename.append("парк");
            }
            if (str.contains("б-р.,"))
            {
              str.remove("б-р.,");
              ename.append("б-р");
            }
            if (str.contains("ш.,"))
            {
              str.remove("ш.,");
              ename.append("ш");
            }
            if (str.contains("сад.,"))
            {
              str.remove("сад.,");
              ename.append("сад");
            }
            if (str.contains("остров.,"))
            {
              str.remove("остров.,");
              ename.append("остров");
            }
            if (str.contains("пл.,"))
            {
              str.remove("пл.,");
              ename.append("пл");
            }
            if (str.contains("аллея.,"))
            {
              str.remove("аллея.,");
              ename.append("аллея");
            }
            if (str.contains("кв-л.,"))
            {
              str.remove("кв-л.,");
              ename.append("кв-л");
            }
            if (str.contains("снт.,"))
            {
              str.remove("снт.,");
              ename.append("снт");
            }
            if (str.contains("тер.,"))
            {
              str.remove("тер.,");
              ename.append("тер");
            }
            if (str.contains("дор.,"))
            {
              str.remove("дор.,");
              ename.append("дор");
            }
            if (str.contains("канал.,"))
            {
              str.remove("канал.,");
              ename.append("канал");
            }
            if (str.contains("метро.,"))
            {
              str.remove("метро.,");
              ename.append("метро");
            }
            if (str.contains("мост.,"))
            {
              str.remove("мост.,");
              ename.append("мост");
            }
            if (str.contains("х.,"))
            {
              str.remove("х.,");
              ename.append("х");
            }

            str = str.trimmed();

            //работа со скобками
            int n1=str.indexOf('(');
            if (n1>0 && (str.indexOf(')',n1)>0))
            {
                int n2=str.indexOf(')', n1);
                int n3=n2-n1;
                additional.append(str.mid(n1+1, n3-1));
                str.remove(n1, n3+1);
            }
        } // конец работы с STR

        /*

        //работаем с B
        if(i==3)
        {
            //приведение к формату: "%n" - %n - число
            if (str.contains("д."))
            {
                str.remove("д.");
            }
            //удаление записи если это не адрес (напр. "а/я" или "нетр..")
            if (str.contains("нетр") || str.contains("а/я")|| str.contains("ая"))
            {
                row.clear();
                return;
            }
            //выделение корпуса (литеры) из содержимого ячейки
            if (str.contains("/"))
            {
                QRegExp reg("/(.+)");
                int pos(0);
                while ((pos = reg.indexIn(str, pos)) != -1) {
                    row[numbKCol].append( reg.cap(1) );
                    pos += reg.matchedLength();
                }
                QString copy = str;
                str.clear();
                copy.remove(reg);
                row[numbBCol].append(copy);
            }

//            // работа с "\"
//            if (str.contains('\\'))
//            {
//               // str.clear();
//                //str.append("1");
//                QRegExp reg("\\(.+)");
//                int pos(0);
//                while ((pos = reg.indexIn(str, pos)) != -1) {
//                    row[4].append( reg.cap(1) );
//                    pos += reg.matchedLength();
//                }
//                QString copy = str;
//                str.clear();
//                copy.remove(reg);
//                row[3].append(copy);
//            }

            QRegExp reg("[а-яА-ЯA-za-z]");
//            QRegExp reg("\\w+");
            if (str.contains(reg))
            {
                int pos(0);
                QString copy1 = row.at(numbKCol);
//                row[numbKCol].clear();
                while ((pos = reg.indexIn(str, pos)) != -1)
                {
                    row[numbKCol].append(copy1+ reg.cap());
                    pos += reg.matchedLength();
                }
//                qDebug() << pos << row[numbKCol];
                QString copy = str;
                str.clear();
                copy.remove(reg);
                row[numbBCol].append(copy);
            }
        }//end work with B

        //работаем с K
        if(i==4)
        {
            //удаление названия элемента ("корп.", "лит." и пр.)
            if (str.contains("ЛИТ."))
            {
                str.remove("ЛИТ.");
            }
            if (str.contains("лит"))
            {
                str.remove("лит");
            }
            if (str.contains("лит."))
            {
                str.remove("лит.");
            }
            if (str.contains("ЛИТЕ"))
            {
                str.remove("ЛИТЕ");
            }
            if (str.contains("ЛИТ-"))
            {
                str.remove("ЛИТ-");
            }
            if (str.contains(",ПОМ."))
            {
                str.remove(",ПОМ.");
            }
            if (str.contains("ЛИТ"))
            {
                str.remove("ЛИТ");
            }
            if (str.contains("ер"))
            {
                str.remove("ер");
            }

            //приведение к формату: "%n|%c" - %n - число %a - буква(-ы)
            QRegExp reg("[^A-Za-zА-Яа-я0-9]*");
            if (str.contains(reg))
            {
                str.remove(reg);
            }

        }

        //работаем с ENAME
        if(i==5)
        {
            //присваивание выделеннного названия структурного элемента из столбца STREET
            //например, ул., пр-т, ш. и пр.
            if(!ename.isEmpty())
                str = ename;
        }

        //работаем с ADD
        if(i==6)
        {
            //присваивание выделенной доп.информации из столбца STREET
            //например, то что содержится в скобках
            if(!additional.isEmpty())
                str = additional;
        }
        */
    }
    //парсинг строки окончен!

    emit rowParsed(_rowNumber, a);
}

void Parser::parseInRow()
{
    qDebug() << "Parser parseInRow" << this->thread()->currentThreadId();
}

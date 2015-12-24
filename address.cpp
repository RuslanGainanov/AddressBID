#include "address.h"

Address::Address():
    _streetId(0),
    _buildId(0)
{
//    qDebug() << "Address constructor";
}

Address::Address(const Address &a)
{
    setStreet(a.getStreet());
    setStreetId(a.getStreetId());
    setBuild(a.getBuild());
    setBuildId(a.getBuildId());
    setAdditional(a.getAdditional());
    setCity1(a.getCity1());
    setCity2(a.getCity2());
    setTypeOfCity2(a.getTypeOfCity2());
    setTypeOfCity1(a.getTypeOfCity1());
    setTypeOfStreet(a.getTypeOfStreet());
    setLitera(a.getLitera());
    setFsubj(a.getFsubj());
    setDistrict(a.getDistrict());
    setKorp(a.getKorp());
//    setEname(a.getEname());
    setRawAddress(a.getRawAddress());
}

void Address::setStreet(const QString &s)
{
    _street=s;
    //работа со скобками
    int n1=_street.indexOf('(');
    if (n1>0 && (_street.indexOf(')',n1)>0))
    {
        int n2=_street.indexOf(')', n1);
        int n3=n2-n1;
        _additional+=_street.mid(n1+1, n3-1);
        _street.remove(n1, n3+1);
    }
}

void Address::setStreetId(const QString sid)
{
    _streetId=sid.toULongLong();
}

void Address::setStreetId(const quint64 sid)
{
    _streetId=sid;
}

quint64 Address::getStreetId() const
{
    return _streetId;
}

QString Address::getStreet() const
{
    return _street;
}

void Address::setKorp(const QString k)
{
    _korp=trim(k);
}

QString Address::getKorp() const
{
    return _korp;
}

void Address::setBuild(const QString b)
{
    _build=trim(b);
}

void Address::setBuildId(const QString bid)
{
    _buildId=bid.toULongLong();
}

void Address::setBuildId(const quint64 bid)
{
    _buildId=bid;
}

quint64 Address::getBuildId() const
{
    return _buildId;
}

QString Address::getBuild() const
{
    return _build;
}

//void Address::setEname(const QString e)
//{
//    _ename=e;
//}

//QString Address::getEname() const
//{
//    return _ename;
//}

void Address::setAdditional(const QString a)
{
    _additional=a;
}

QString Address::getAdditional() const
{
    return _additional;
}

void Address::setCity1(const QString c)
{
    _city1=c;
    //работа со скобками
    int n1=_city1.indexOf('(');
    if (n1>0 && (_city1.indexOf(')',n1)>0))
    {
        int n2=_city1.indexOf(')', n1);
        int n3=n2-n1;
        _additional+=_city1.mid(n1+1, n3-1);
        _city1.remove(n1, n3+1);
    }
}

QString Address::getCity1() const
{
    return _city1;
}

void Address::setCity2(const QString c)
{
    _city2=c;
    //работа со скобками
    int n1=_city2.indexOf('(');
    if (n1>0 && (_city2.indexOf(')',n1)>0))
    {
        int n2=_city2.indexOf(')', n1);
        int n3=n2-n1;
        _additional+=_city2.mid(n1+1, n3-1);
        _city2.remove(n1, n3+1);
    }
}

QString Address::getCity2() const
{
    return _city2;
}

void Address::setTypeOfCity1(const QString t)
{
    _typeOfCity1=t;
}

QString Address::getTypeOfCity1() const
{
    return _typeOfCity1;
}
void Address::setTypeOfCity2(const QString t)
{
    _typeOfCity2=t;
}

QString Address::getTypeOfCity2() const
{
    return _typeOfCity2;
}

void Address::setTypeOfStreet(const QString t)
{
    _typeOfStreet=t;
}

QString Address::getTypeOfStreet() const
{
    return _typeOfStreet;
}

void Address::setLitera(const QString l)
{
    _litera=trim(l);
}

QString Address::getLitera() const
{
    return _litera;
}

void Address::setDistrict(const QString d)
{
    _district=d;
}

QString Address::getDistrict() const
{
    return _district;
}

void Address::setFsubj(const QString f)
{
    _fsubj=f;
}

QString Address::getFsubj() const
{
    return _fsubj;
}


QString Address::toString(TypeOfData t) const
{
    QString res("*** Address: ***\n");
    if(!isEmpty())
    {
        if(t==RAW)
        {
            res += getRawAddress().join(';');
        }
        else if(t==PARSED)
        {
            res += "StreetID: " + QString::number(getStreetId()) + "\n";
            res += "BuildID: " + QString::number(getBuildId()) + "\n";
            res += "Federal subj: " + getFsubj() + "\n";
            res += "District: " + getDistrict() + "\n";
            res += "Type of City1: " + getTypeOfCity1() + "\n";
            res += "City1: " + getCity1() + "\n";
            res += "Type of City2: " + getTypeOfCity2() + "\n";
            res += "City2: " + getCity2() + "\n";
            res += "Type of Street: " + getTypeOfStreet() + "\n";
            res += "Street: " + getStreet() + "\n";
            res += "Build: " + getBuild() + "\n";
            res += "Korpus: " + getKorp() + "\n";
            res += "Litera: " + getLitera() + "\n";
            res += "Correct: " + QString(isCorrect()?"1":"0") + "\n";
        }
    }
    else
    {
        res + "Is empty\n";
    }
    return res;
}

//QStringList Address::toDebug(TypeOfData t) const
//{
//    QStringList list;
//    list << "*** Address: ***";
//    if(!isEmpty())
//    {
//        if(t==RAW)
//        {
//            list << getRawAddress();
//        }
//        else if(t==PARSED)
//        {
//            list << "Street" << getStreet() << "\n";
//            list << "Korpus" << getKorp() << "\n";
//            list << "Build" << getBuild() << "\n";
//        }
//    }
//    else
//    {
//        list << "is empty\n";
//    }
//    return list;
//}

void Address::clear()
{
    _rawAddress.clear();
    _street.clear();
    _korp.clear();
    _build.clear();
//    _ename.clear();
    _additional.clear();
    _city1.clear();
    _city2.clear();
    _typeOfCity1.clear();
    _typeOfCity2.clear();
    _typeOfStreet.clear();
    _litera.clear();
    _district.clear();
    _fsubj.clear();
}

bool Address::isEmpty() const
{
    return (getRawAddress().isEmpty() &&
            getStreet().isEmpty() &&
            getBuild().isEmpty() &&
            getKorp().isEmpty() &&
//            getEname().isEmpty() &&
            getAdditional().isEmpty() &&
            getCity1().isEmpty() &&
            getDistrict().isEmpty() &&
            getLitera().isEmpty() &&
            getTypeOfCity1().isEmpty() &&
            getTypeOfCity2().isEmpty() &&
            getTypeOfStreet().isEmpty() &&
            getFsubj().isEmpty());
}

void Address::setRawAddress(const QString &str)
{
    if(str.isEmpty())
        return;
    QString s(str);
    if((*(s.begin())) == '\"')
        s.remove(0,1);
    if((*(s.end()-1)) == '\"')
        s.remove(s.size()-1,1);
    _rawAddress = s.split("\";\"");
    _buildId = _rawAddress.at(0).toULongLong();
    _streetId = _rawAddress.at(2).toULongLong();
}

void Address::setRawAddress(const QStringList &row)
{
    _rawAddress=row;
}

QStringList Address::getRawAddress() const
{
    return _rawAddress;
}

void Address::setCorrect(const bool c)
{
    _isCorrect=c;
}

bool Address::isCorrect() const
{
    return _isCorrect;
}


QString Address::trim(QString s)
{
    return s.trimmed();
}

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
    setCity(a.getCity());
    setCity2(a.getCity2());
    setTypeOfCity2(a.getTypeOfCity2());
    setTypeOfCity1(a.getTypeOfCity1());
    setTypeOfStreet(a.getTypeOfStreet());
    setLitera(a.getLitera());
    setFsubj(a.getFsubj());
    setDistrict(a.getDistrict());
    setKorp(a.getKorp());
    setEname(a.getEname());
    setRawAddress(a.getRawAddress());
}

void Address::setStreet(const QString s)
{
    _street=s;
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
    _korp=k;
}

QString Address::getKorp() const
{
    return _korp;
}

void Address::setBuild(const QString b)
{
    _build=b;
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

void Address::setEname(const QString e)
{
    _ename=e;
}

QString Address::getEname() const
{
    return _ename;
}

void Address::setAdditional(const QString a)
{
    _additional=a;
}

QString Address::getAdditional() const
{
    return _additional;
}

void Address::setCity(const QString c)
{
    _city=c;
}

QString Address::getCity() const
{
    return _city;
}

void Address::setCity2(const QString c)
{
    _city2=c;
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
    _litera=l;
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
            res += getRawAddress().join("\n");
        }
        else if(t==PARSED)
        {
            res += "Federal subj: " + getFsubj() + "\n";
            res += "District: " + getDistrict() + "\n";
            res += "Type of City: " + getTypeOfCity1() + "\n";
            res += "City: " + getCity() + "\n";
            res += "Type of City2: " + getTypeOfCity2() + "\n";
            res += "City: " + getCity2() + "\n";
            res += "Ename: " + getEname() + "\n";
            res += "Type of Street: " + getTypeOfStreet() + "\n";
            res += "Street: " + getStreet() + "\n";
            res += "Build: " + getBuild() + "\n";
            res += "Korpus: " + getKorp() + "\n";
            res += "Litera: " + getLitera() + "\n";
        }
    }
    else
    {
        res + "Is empty\n";
    }
    return res;
}

QStringList Address::toDebug(TypeOfData t) const
{
    QStringList list;
    list << "*** Address: ***";
    if(!isEmpty())
    {
        if(t==RAW)
        {
            list << getRawAddress();
        }
        else if(t==PARSED)
        {
            list << "Street" << getStreet() << "\n";
            list << "Korpus" << getKorp() << "\n";
            list << "Build" << getBuild() << "\n";
        }
    }
    else
    {
        list << "is empty\n";
    }
    return list;
}

void Address::clear()
{
    _rawAddress.clear();
    _street.clear();
    _korp.clear();
    _build.clear();
    _ename.clear();
    _additional.clear();
    _city.clear();
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
            getEname().isEmpty() &&
            getAdditional().isEmpty() &&
            getCity().isEmpty() &&
            getDistrict().isEmpty() &&
            getLitera().isEmpty() &&
            getTypeOfCity1().isEmpty() &&
            getTypeOfCity2().isEmpty() &&
            getTypeOfStreet().isEmpty() &&
            getFsubj().isEmpty());
}

void Address::setRawAddress(const QStringList &row)
{
    _rawAddress=row;
}

QStringList Address::getRawAddress() const
{
    return _rawAddress;
}

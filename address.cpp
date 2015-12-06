#include "address.h"

Address::Address()
{
//    qDebug() << "Address constructor";
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

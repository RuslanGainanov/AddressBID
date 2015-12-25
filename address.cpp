#include "address.h"

Address::Address():
    _streetId(0),
    _buildId(0),
    _typeFSubj(INCORRECT_SUBJ)
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
    setTypeOfFSubj(a.getTypeOfFSubj());
    setFsubj(a.getFsubj());
    setDistrict(a.getDistrict());
    setKorp(a.getKorp());
    setRawAddress(a.getRawAddress());
}

void Address::setStreet(const QString &s)
{
    _street=toLower(trim(s));
    workWithBranches(_street);
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
    _korp=toLower(trim(k));
}

QString Address::getKorp() const
{
    return _korp;
}

void Address::setBuild(const QString b)
{
    _build=b;
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
    _additional=toLower(trim(a));
}

QString Address::getAdditional() const
{
    return _additional;
}

void Address::setCity1(const QString c)
{
    _city1=toLower(trim(c));
    workWithBranches(_city1);
}

QString Address::getCity1() const
{
    return _city1;
}

void Address::setCity2(const QString c)
{
    _city2=toLower(trim(c));
    workWithBranches(_city2);
}

QString Address::getCity2() const
{
    return _city2;
}

void Address::setTypeOfCity1(const QString t)
{
    _typeOfCity1=toLower(trim(t));
}

QString Address::getTypeOfCity1() const
{
    return _typeOfCity1;
}
void Address::setTypeOfCity2(const QString t)
{
    _typeOfCity2=toLower(trim(t));
}

QString Address::getTypeOfCity2() const
{
    return _typeOfCity2;
}

void Address::setTypeOfStreet(const QString t)
{
    _typeOfStreet=toLower(trim(t));
}

QString Address::getTypeOfStreet() const
{
    return _typeOfStreet;
}

void Address::setLitera(const QString l)
{
    _litera=toLower(trim(l));
}

QString Address::getLitera() const
{
    return _litera;
}

void Address::setDistrict(const QString d)
{
    _district=toLower(trim(d));
}

QString Address::getDistrict() const
{
    return _district;
}


void Address::setTypeOfFSubj(const QString t)
{
    QString s = toLower(trim(t));
//    qDebug().noquote() << "setTypeOfFSubj:" << QString("[%1] -> [%2]").arg(t).arg(s);
    QMap<QString, TypeOfFederalSubject>::const_iterator i = MapStringFSubj.find(s);
    if (i != MapStringFSubj.end() && i.key() == s) {
        _typeFSubj=i.value();
    }
    else
        _typeFSubj=INCORRECT_SUBJ;
}

void Address::setTypeOfFSubj(const TypeOfFederalSubject t)
{
    _typeFSubj=t;
}

TypeOfFederalSubject Address::getTypeOfFSubj() const
{
    return _typeFSubj;
}

QString Address::getTypeOfFSubjInString() const
{
    return MapFSubjString.value(_typeFSubj, "");
}

void Address::setFsubj(const QString f)
{
    _fsubj=toLower(trim(f));
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
            res += "Type of Federal Subj: " + getTypeOfFSubjInString() + "\n";
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

QString Address::trim(QString str) const
{
    str.remove(QRegExp("^\\s*[\\.,;()]*"));
    str.remove(QRegExp("[\\.,;()]*\\s*$"));
    return str;
}

QString Address::toLower(QString str) const
{
    return str.toLower();
}

void Address::workWithBranches(QString &s)
{
    int n1=s.indexOf('(');
    if (n1>0 && (s.indexOf(')',n1)>0))
    {
        int n2=s.indexOf(')', n1);
        int n3=n2-n1;
        _additional+=s.mid(n1+1, n3-1);
        s.remove(n1, n3+1);
    }
}

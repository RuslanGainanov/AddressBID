#ifndef ADDRESS_H
#define ADDRESS_H

#include <QString>
#include <QStringList>

enum TypeOfData
{
    RAW = 1,
    PARSED = 2
};


class Address
{
public:
    Address();
    Address(const Address &a);
    void setStreet(const QString &s);
    QString getStreet() const;

    void setStreetId(const QString sid);
    void setStreetId(const quint64 sid);
    quint64 getStreetId() const;

    void setKorp(const QString k);
    QString getKorp() const;

    void setBuild(const QString b);
    QString getBuild() const;

    void setBuildId(const QString bid);
    void setBuildId(const quint64 bid);
    quint64 getBuildId() const;

    void setAdditional(const QString a);
    QString getAdditional() const;

    void setCity1(const QString c);
    QString getCity1() const;

    void setCity2(const QString c);
    QString getCity2() const;

    void setTypeOfCity1(const QString c);
    QString getTypeOfCity1() const;

    void setTypeOfCity2(const QString c);
    QString getTypeOfCity2() const;

    void setTypeOfStreet(const QString c);
    QString getTypeOfStreet() const;

    void setLitera(const QString c);
    QString getLitera() const;

    void setDistrict(const QString d);
    QString getDistrict() const;

    void setFsubj(const QString f);
    QString getFsubj() const;

    QString toString(TypeOfData t=RAW) const;
//    QStringList toDebug(TypeOfData t=RAW) const;
    bool isEmpty() const;
    void clear();

    void setRawAddress(const QString &str);
    void setRawAddress(const QStringList &row);
    QStringList getRawAddress() const;

    void setCorrect(const bool c);
    bool isCorrect() const;

private:
    QStringList _rawAddress;///< строка адреса как она есть
    QString _street;        ///< название ул. (пр., ш., бул. и пр.)
    quint64 _streetId;      ///< id ул.
    QString _korp;          ///< корпус
    QString _build;         ///< дом
    quint64 _buildId;       ///< id дома
//    QString _ename;         ///< element name (элементарное имя) (ул., пр., ш., бул. и пр.)
    QString _additional;    ///< допольнительная информация

    QString _typeOfStreet;
    QString _typeOfCity1;
    QString _typeOfCity2;
    QString _city2;         ///< город2
    QString _litera;        ///< литера
    QString _city1;         ///< город1
    QString _district;      ///< район
    QString _fsubj;         ///< federal subject (федеральный субъект) (название области, края, республики и пр.)
    bool    _isCorrect;     ///< флаг устанавливается в 1, если адрес был распарсен
};

#endif // ADDRESS_H

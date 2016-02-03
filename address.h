#ifndef ADDRESS_H
#define ADDRESS_H

#include <QList>
#include <QString>
#include <QStringList>
#include "defines.h"

// ********************* class Address ********************
/**
 * \class Address
 * \brief Класс для работы с данными адреса
 */
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

    void setTypeOfFSubj(const QString t);
    void setTypeOfFSubj(const TypeOfFederalSubject t);
    TypeOfFederalSubject getTypeOfFSubj() const;
    QString getTypeOfFSubjInString() const;

    void setFsubj(const QString f);
    QString getFsubj() const;

    QString toDebug() const;
    QString toCsv() const;
    QString toInsertSqlQuery() const;
    bool isEmpty() const;
    void clear();

    void setRawAddress(const QString &str);
    void setRawAddress(const QStringList &row);
    QStringList getRawAddress() const;
    QString getRawAddressString() const;
    QString getRawAddressStringWithoutID() const;

    void setCorrect(const bool c);
    bool isCorrect() const;


private:
    QStringList _rawAddress;///< строка адреса как она есть
    QString _street;        ///< название ул. (пр., ш., бул. и пр.)
    quint64 _streetId;      ///< id ул.
    QString _korp;          ///< корпус
    QString _build;         ///< дом
    quint64 _buildId;       ///< id дома
    QString _additional;    ///< допольнительная информация

    QString _typeOfStreet;  ///< тип улицы (ул., пр-кт., бул., ш. и пр.) (без точки на конце)
    QString _typeOfCity1;   ///< тип города1 (д., п., г., нп. и пр.) (без точки на конце)
    QString _typeOfCity2;   ///< тип города2 (д., п., г., нп. и пр.) (без точки на конце)
    QString _city2;         ///< город2
    QString _litera;        ///< литера
    QString _city1;         ///< город1
    QString _district;      ///< район
    TypeOfFederalSubject _typeFSubj; ///< тип федерального субъекта РФ (область, край, республика, АО, АОбл и пр.)
    QString _fsubj;         ///< federal subject (название федерального субъекта)
    bool    _isCorrect;     ///< флаг устанавливается в 1, если адрес был успешно распарсен

    /**
     * \fn QString trim(QString str) const
     * \brief Функция удаления незначащих символов (знаков пунктуации, пробелов и пр.) в начале и в конце строки
     */
    QString trim(QString str) const;

    /**
     * \fn QString toLower(QString str) const
     * \brief Функция приведения строки к нижнему регистру
     */
    QString toLower(QString str) const;

    /**
     * \fn void workWithBranches(QString &s)
     * \brief Функция работы со скобками в строке
     *
     * Переносит все, что содержится в скобках в поле "допольнительная информация"
     */
    void workWithBranches(QString &s);

    /**
     * \fn void convertToCorrectTypeOfStreet(QString &s) const
     * \brief Функция приведения типа улицы к корректному значению
     */
    void convertToCorrectTypeOfStreet(QString &s) const;

};

typedef QList< Address > ListAddress;

#endif // ADDRESS_H

#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QThread>
#include <QMutex>
#include <QMap>

enum TypeOfRow
{
    BASE = 1,
    INPUT = 2
};

enum TypeOfData
{
    RAW = 1,
    PARSED = 2
};

enum AddressElements
{
    STREET=1,        ///< название ул. (пр., ш., бул. и пр.)
    KORP,          ///< корпус
    BUILD,         ///< дом
    ENAME,         ///< element name (элементарное имя) (ул., пр., ш., бул. и пр.)
    ADDITIONAL,    ///< допольнительная информация

    CITY,          ///< город
    DISTRICT,      ///< район
    FSUBJ,         ///< federal subject (федеральный субъект) (название области, края, республики и пр.)
};

const QList<AddressElements> ListAddressElements {
    STREET, KORP, BUILD, ENAME, ADDITIONAL, CITY, DISTRICT, FSUBJ
};

const QMap<AddressElements, QString> MapColumnNames {
    {STREET, "STREET"},
    {KORP, "KORP"},
    {BUILD, "BUILD"},
    {ENAME, "ENAME"},
    {ADDITIONAL, "ADD"},
    {CITY, "CITY"},
    {DISTRICT, "DISTRICT"},
    {FSUBJ, "FSUBJ"},
};

#endif // DEFINES_H

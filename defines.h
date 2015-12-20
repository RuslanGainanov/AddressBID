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
#include <QSet>
#include <QDateTime>
#include <QTextStream>
#include <assert.h>

const int MAX_OPEN_ROWS=0;
//const int MAX_OPEN_IN_ROWS=0; //moved in ExcelWidget

const QString BaseRegPattern =
        "\"([0-9]*)\";\"(г*\\.* *[^ ]*( обл\\.| АО\\.| Респ\\.))*(, )*((.*) р-н)*(, )*((п\\/о\\.) ([^,]*))*,* *((д\\.|ж\\/д_рзд\\.|рп\\.|ж\\/д_ст\\.|ст\\.|высел\\.|х\\.|казарма\\.|м\\.|жилрайон\\.|нп\\.|снт\\.|г\\.|дп\\.|снт\\.|п\\.|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|с\\.|тер\\.|массив\\.) ([^,]*))*(, )*((д\\.|ж\\/д_рзд\\.снт\\.|г\\.|дп\\.|нп\\.|мкр\\.|жилзона\\.|снт\\.|п\\.|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|с\\.|тер\\.|массив\\.)( [^,]*))*(, )*((.* )(ул\\.|пер\\.|наб\\.|ш\\.|линия\\.|казарма\\.|б-р\\.|тер\\.|уч-к\\.|пр-кт\\.|сад\\.|остров\\.|канал\\.|дор\\.|км\\.|городок\\.|тракт\\.|гск\\.|просек\\.|переезд\\.|заезд\\.|парк\\.|площадь|проезд\\.|туп\\.|м\\.|проток\\.|ст\\.|вал\\.|п\\.|спуск\\.|пл-ка\\.|д\\.|коса\\.|п\\/ст\\.|пр\\.|пл\\.|мкр\\.|аллея\\.|кв-л\\.|ж\\/д_будка\\.|х\\.|рзд\\.|тск\\.|сквер\\.|стр\\.|снт\\.|мост\\.|метро\\.))*(, )*\";\"([0-9]*)\";\"((д\\.|ая\\.|ая|нетр\\.|нетр|а\\\\я\\.|а\\\\я|а\\/я\\.|а\\/я)*(([0-9Х][-\\\\\\/0-9]*)([\\\\\\/-]([А-Яа-я])*)*))*([^\"]*)\";\"(([0-9]*)([А-Яа-я]*))*\"";

enum TypeOfRow
{
    BASE_TYPE = 1,
    INPUT_TYPE = 2
};

enum AddressElements
{
    STREET=1,      ///< название ул. (пр., ш., бул. и пр.)
    STREET_ID,     ///< id ул.
    BUILD,         ///< дом
    BUILD_ID,      ///< id дома
    KORP,          ///< корпус (цифра)
    LITERA,        ///< литера (буква)
    ADDITIONAL,    ///< допольнительная информация (все что в скобках)

    TYPE_OF_CITY1, ///< тип основного города
    CITY1,         ///< основной город
    TYPE_OF_CITY2, ///< тип доп. города (д., п., с., и т.д.)
    CITY2,         ///< доп. город
    TYPE_OF_STREET,///< (ул., пр., ш., бул. и пр.)
    DISTRICT,      ///< район
    FSUBJ,         ///< federal subject (федеральный субъект) (название области, края, республики и пр.)
    RAW_ADDR,      ///< "сырой" адрес (тот который был прочитан из файла сsv)
    CORRECT,       ///< корректный ли адрес в структуре (был ли распарсен) (1-да)
};

const QList<AddressElements> ListMandatoryAddressElements {
    STREET, STREET_ID, BUILD_ID
};

const QList<AddressElements> ListAddressElements {
    STREET, STREET_ID, KORP, BUILD, BUILD_ID, ADDITIONAL,
            DISTRICT, FSUBJ,
            TYPE_OF_CITY1,
            CITY1,
            TYPE_OF_CITY2,
            CITY2,
            TYPE_OF_STREET,
            LITERA,
            CORRECT,
};

typedef QMap<AddressElements, int> MapAddressElementPosition;
typedef QMap<AddressElements, QString> MapAEValue;

//key of map - sheet name, value - list of rows, row - string list
typedef QList< QStringList > ExcelSheet;
typedef QMap<QString, ExcelSheet > ExcelDocument;

typedef QVector<QString> VectorOfString;

const QMap<AddressElements, QString> MapColumnNames {
    {STREET, "STREET"},
    {STREET_ID, "STREET_ID"},
    {KORP, "KORP"},
    {BUILD, "BUILD"},
    {BUILD_ID, "BUILD_ID"},
    {ADDITIONAL, "ADDITIONAL"},
    {DISTRICT, "DISTRICT"},
    {FSUBJ, "FSUBJ"},
    {RAW_ADDR, "RAW"},
    {TYPE_OF_CITY1, "TYPE_OF_CITY1"},
    {CITY1,	"CITY1"},
    {TYPE_OF_CITY2, "TYPE_OF_CITY2"},
    {CITY2, "CITY2"},
    {TYPE_OF_STREET, "TYPE_OF_STREET"},
    {LITERA, "LITERA"},
    {CORRECT, "CORRECT"},
};

const QMap<AddressElements, QString> MapColumnParsedNames {
    {FSUBJ, "!P_FSUBJ"},
    {DISTRICT, "!P_DISTRICT"},
    {TYPE_OF_CITY1, "!P_TYPE_OF_CITY1"},
    {CITY1, "!P_CITY1"},
    {TYPE_OF_CITY2, "!P_TYPE_OF_CITY2"},
    {CITY2, "!P_CITY2"},
    {TYPE_OF_STREET, "!P_TYPE_OF_STREET"},
    {STREET, "!P_STREET"},
    {STREET_ID, "!P_STRT_ID"},
    {BUILD, "!P_BLD"},
    {BUILD_ID, "!P_BLD_ID"},
    {KORP, "!P_KORP"},
    {LITERA, "!P_LITERA"},
    {ADDITIONAL, "!P_ADDITIONAL"},
};

enum Errors
{
    XLS_WORKER_ERROR=1,
};

#endif // DEFINES_H

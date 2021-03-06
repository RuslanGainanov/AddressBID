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

const QString TableName = "base1";
const QString DefaultBaseName = "base1.db";

const int MAX_OPEN_ROWS=0;

const QString BaseRegPattern =
        "\"([0-9]*)\";\"( *([^ ]*)( обл\\.| ао\\.| респ\\.))*(, )*((.*) р-н)*,* *((п\\/о\\.|д\\.|ж\\/д_рзд\\.|рп\\.|ж\\/д_ст\\.|ст\\.|высел\\.|х\\.|казарма\\.|м\\.|жилрайон\\.|нп\\.|снт\\.|г\\.|дп\\.|снт\\.|п\\.|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|с\\.|тер\\.|массив\\.) ([^,]*))*,* *((п\\/о\\.|д\\.|ж\\/д_рзд\\.снт\\.|г\\.|дп\\.|нп\\.|мкр\\.|жилзона\\.|снт\\.|п\\.|пст\\.|п\\/ст\\.|пгт\\.|п\\/гт\\.|с\\.|тер\\.|массив\\.)( [^,]*))*(, )*((.* )(ул\\.|пер\\.|наб\\.|ш\\.|линия\\.|казарма\\.|б-р\\.|тер\\.|уч-к\\.|пр-кт\\.|сад\\.|остров\\.|канал\\.|дор\\.|км\\.|городок\\.|тракт\\.|гск\\.|просек\\.|переезд\\.|заезд\\.|парк\\.|площадь|проезд\\.|туп\\.|м\\.|проток\\.|ст\\.|вал\\.|п\\.|спуск\\.|пл-ка\\.|д\\.|коса\\.|п\\/ст\\.|пр\\.|пл\\.|мкр\\.|аллея\\.|кв-л\\.|ж\\/д_будка\\.|х\\.|рзд\\.|тск\\.|сквер\\.|стр\\.|снт\\.|мост\\.|метро\\.))*(, )*\";\"([0-9]*)\";\"((д\\.|ая\\.|ая|нетр\\.|нетр|а\\\\я\\.|а\\\\я|а\\/я\\.|а\\/я)*(([0-9х][-\\\\\\/0-9]*)([\\\\\\/-]([а-я])*)*))*([^\"]*)\";\"(([0-9]*)([а-я]*))*\"";
const QString PatternForNotCorrect=
        "\"([0-9]*)\";\"([^\"]*)\";\"([0-9]*)\";\"([^\"]*)\";\"([^\"]*)\"";

//const int ListAddressMemoryReserve=800000;

enum AddressElements
{
    STREET=1,      ///< название ул. (пр., ш., бул. и пр.)
    STREET_ID,     ///< id ул.
    KORP,          ///< корпус
    BUILD,         ///< дом
    BUILD_ID,      ///< id дома
    ADDITIONAL,    ///< допольнительная информация

    TYPE_OF_CITY1, ///< тип города1 (д., п., г., нп. и пр.)
    CITY1,         ///< город1
    TYPE_OF_CITY2, ///< тип города2 (д., п., г., нп. и пр.)
    CITY2,         ///< город2
    TYPE_OF_STREET,///< тип улицы (ул., пр-кт., бул., ш. и пр.)
    LITERA,        ///< литера
    DISTRICT,      ///< район
    TYPE_OF_FSUBJ, ///< тип федерального субъекта РФ (область, край, республика, АО, АОбл и пр.)
    FSUBJ,         ///< federal subject (федеральный субъект) (название области, края, республики и пр.)
    RAW_ADDR,      ///< "сырой" адрес (тот который был прочитан из файла сsv)
    CORRECT,       ///< флаг устанавливается в 1, если адрес был успешно распарсен
};

const QList<AddressElements> ListMandatoryAddressElements {
    STREET, STREET_ID, BUILD_ID
};

const QList<AddressElements> ListAddressElements {
    STREET, STREET_ID, KORP, BUILD, BUILD_ID, ADDITIONAL,
    DISTRICT, FSUBJ, TYPE_OF_CITY1, CITY1, TYPE_OF_CITY2,
    CITY2, TYPE_OF_STREET, LITERA, CORRECT, TYPE_OF_FSUBJ,
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
    {TYPE_OF_FSUBJ, "TYPE_OF_FSUBJ"},
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
    {STREET, "!P_STREET"},
    {STREET_ID, "!P_STREET_ID"},
    {BUILD, "!P_BUILD"},
    {BUILD_ID, "!P_BUILD_ID"},
    {TYPE_OF_STREET, "!P_TYPE_OF_STREET"},
    {ADDITIONAL, "!P_ADDITIONAL"},
    {TYPE_OF_CITY1, "!P_TYPE_OF_CITY1"},
    {CITY1, "!P_CITY1"},
    {TYPE_OF_CITY2, "!P_TYPE_OF_CITY2"},
    {CITY2, "!P_CITY2"},
    {TYPE_OF_FSUBJ, "!P_TYPE_OF_FSUBJ"},
    {FSUBJ, "!P_FSUBJ"},
    {DISTRICT, "!P_DISTRICT"},
    {TYPE_OF_STREET, "!P_TYPE_OF_STREET"},
    {STREET, "!P_STREET"},
    {KORP, "!P_KORP"},
    {LITERA, "!P_LITERA"},
    {ADDITIONAL, "!P_ADDITIONAL"},
    {RAW_ADDR, "!P_RAW_ADDR"},

};

/**
 * \enum TypeOfFederalSubject
 * \brief Тип федерального субъекта
 *
 * Список возможных типов федеральных субъектов. Описание:
 * https://en.wikipedia.org/wiki/Federal_subjects_of_Russia
 * Примечание: тип 'CITY_OF_FED_SIGN' не используется
 */
enum TypeOfFederalSubject
{
    INCORRECT_SUBJ = -1,    ///< Некорректный тип
    REPUBLIC = 1,           ///< Республика
    KRAY = 2,               ///< Край
    OBLAST = 3,             ///< Область
    CITY_OF_FED_SIGN = 4,   ///< Город федерального значения
    AUTONOM_OBLAST = 5,     ///< Автономная область
    AUTONOM_OKRUG = 6       ///< Автономный округ
};

const QMap<QString, TypeOfFederalSubject> MapStringFSubj {
    {"республика", REPUBLIC},
    {"респ", REPUBLIC},                 //main REPUBLIC
    {"край", KRAY},                     //main KRAY
    {"кр", KRAY},
    {"область", OBLAST},
    {"обл", OBLAST},                    //main OBLAST
    {"автономная область", AUTONOM_OBLAST},
    {"автономная обл", AUTONOM_OBLAST},
    {"авт.обл", AUTONOM_OBLAST},
    {"аобл", AUTONOM_OBLAST},           //main AUTONOM_OBLAST
    {"автономный округ", AUTONOM_OKRUG},
    {"а.о", AUTONOM_OKRUG},
    {"ао", AUTONOM_OKRUG},              //main AUTONOM_OKRUG
};

const QMap<TypeOfFederalSubject, QString> MapFSubjString {
    {REPUBLIC, "респ"},
    {KRAY, "край"},
    {OBLAST, "обл"},
    {AUTONOM_OBLAST, "аобл"},
    {AUTONOM_OKRUG, "ао"},
    {INCORRECT_SUBJ, "inc"}
};

#endif // DEFINES_H

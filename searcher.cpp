#include "searcher.h"

Searcher::Searcher(const QString &name,
                   QObject *parent) :
    QObject(parent)
  , _numberRowsForProcessing(0)
  , _sheet(name)
  , _canceled(false)
  , _isFinished(false)
{
    qDebug() << "  Searcher()" << _sheet;
}

Searcher::~Searcher()
{
    qDebug() << "  ~Searcher()" << _sheet;
}

void Searcher::cancel()
{
//    qDebug() << "  Searcher::cancel()" << _sheet;
    _canceled=true;
    emit finished(_sheet);
    _isFinished=true;
}

bool Searcher::isCanceled()
{
    return _canceled;
}

//static function:
SearchStruct Searcher::concSelectAddress(SearchStruct s)
{
//    qDebug() << "Searcher::concSelectAddress <" << s.row << QThread::currentThreadId();
    if(!s.findIt)
        return s;
    Address &a=s.a;

    QString str = QString("SELECT STREET_ID, BUILD_ID, RAW "
                          "FROM %1 "
                          "WHERE TYPE_OF_FSUBJ = '%2' "
                          "  AND FSUBJ = '%3' "
                          "  AND DISTRICT = '%4' "
                          "  AND TYPE_OF_CITY1 = '%5' "
                          "  AND CITY1 = '%6' "
                          "  AND TYPE_OF_CITY2 = '%7' "
                          "  AND CITY2 = '%8'"
                          "  AND TYPE_OF_STREET = '%9' "
                          "  AND STREET = '%10' "
                          "  AND BUILD = '%11' "
                          "  AND KORP = '%12' "
                          "  AND LITERA = '%13' "
                          "  AND CORRECT = '1'; "
                          )
                  .arg(TableName)
                  .arg(a.getTypeOfFSubjInString())
                  .arg(a.getFsubj())
                  .arg(a.getDistrict())
                  .arg(a.getTypeOfCity1())
                  .arg(a.getCity1())
                  .arg(a.getTypeOfCity2())
                  .arg(a.getCity2())
                  .arg(a.getTypeOfStreet())
                  .arg(a.getStreet())
                  .arg(a.getBuild())
                  .arg(a.getKorp())
                  .arg(a.getLitera());
//    qDebug() << "Searcher::selectAddress" << str;
    QSqlQuery query;
//    qDebug() << "Searcher::selectAddress BEGIN" << query.isActive() << s.row;
    if (!query.exec(str))
        return s;
//    qDebug() << "Searcher::selectAddress END" << query.isActive() << s.row;

    //Reading of the data
//    qDebug() << "Searcher::Reading record BEGIN" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QSqlRecord rec = query.record();
//    qDebug() << "Searcher::Reading record END" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
//    qDebug() << "Searcher::Reading record count" << rec.count();
    bool isExists=false;
    while (query.next()) {
        isExists=true;
//        qDebug() << "Searcher::Reading query isValid" << query.isValid();
        a.setRawAddress( query.value(rec.indexOf("RAW")).toString() );
        a.setBuildId( query.value(rec.indexOf("BUILD_ID")).toULongLong() );
        a.setStreetId( query.value(rec.indexOf("STREET_ID")).toULongLong() );
    }
//    qDebug() << "Searcher::Reading next END" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    s.found=isExists;

//    qDebug() << "Searcher::concSelectAddress >" << s.row << QThread::currentThreadId();
    return s;
}

void Searcher::selectAddress(QString sheet, int nRow, Address a, bool findIt)
{
    if(!findIt)
        return;
    if(_canceled)
    {
        if(_isFinished)
            return;
        emit finished(_sheet);
        _isFinished=true;
    }

    QString str = QString("SELECT STREET_ID, BUILD_ID, RAW "
                          "FROM %1 "
                          "WHERE TYPE_OF_FSUBJ = '%2' "
                          "  AND FSUBJ = '%3' "
                          "  AND DISTRICT = '%4' "
                          "  AND TYPE_OF_CITY1 = '%5' "
                          "  AND CITY1 = '%6' "
                          "  AND TYPE_OF_CITY2 = '%7' "
                          "  AND CITY2 = '%8'"
                          "  AND TYPE_OF_STREET = '%9' "
                          "  AND STREET = '%10' "
                          "  AND BUILD = '%11' "
                          "  AND KORP = '%12' "
                          "  AND LITERA = '%13' "
                          "  AND CORRECT = '1'; "
                          )
                  .arg(TableName)
                  .arg(a.getTypeOfFSubjInString())
                  .arg(a.getFsubj())
                  .arg(a.getDistrict())
                  .arg(a.getTypeOfCity1())
                  .arg(a.getCity1())
                  .arg(a.getTypeOfCity2())
                  .arg(a.getCity2())
                  .arg(a.getTypeOfStreet())
                  .arg(a.getStreet())
                  .arg(a.getBuild())
                  .arg(a.getKorp())
                  .arg(a.getLitera());
//    qDebug() << "Searcher::selectAddress" << str;
    QSqlQuery query;
//    qDebug() << "Searcher::selectAddress BEGIN" << nRow;
    if (!query.exec(str))
    {
        emit toDebug(objectName(),
                     QString("Невозможно выполнить запрос '%1'. Ошибка: '%2'")
                     .arg(str).arg(query.lastError().text()));
        return;
    }
//    qDebug() << "Searcher::selectAddress END" << nRow;

    //Reading of the data
    QSqlRecord rec     = query.record();
    bool isExists=false;
    while (query.next()) {
        isExists=true;
        a.setRawAddress( query.value(rec.indexOf("RAW")).toString() );
        a.setBuildId( query.value(rec.indexOf("BUILD_ID")).toULongLong() );
        a.setStreetId( query.value(rec.indexOf("STREET_ID")).toULongLong() );
        emit addressFounded(sheet, nRow, a);
    }
    if(!isExists)
        emit addressNotFounded(sheet, nRow, a);

    if(nRow+1>=_numberRowsForProcessing)
    {
        if(_isFinished)
            return;
        emit finished(_sheet);
        _isFinished=true;
    }
}

void Searcher::setCountRows(int rows)
{
    _numberRowsForProcessing=rows;
    _canceled=false;
}

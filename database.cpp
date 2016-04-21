#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent),
    _model(nullptr),
    _connected(false),
    _canceled(false),
    _countNotInsertedRows(0)
{
    setObjectName("Database");
}

Database::~Database()
{
    qDebug() << "  ~Database() <";
    if(_model!=nullptr)
        delete _model;
    qDebug() << "  ~Database() >";
}

void Database::setBaseName(QString name)
{
    _baseName = name;
}

QString Database::baseName()
{
    return _baseName;
//    return DefaultBaseName;
}

void Database::cancelInsertOperation()
{
    _canceled=true;
}

bool Database::isCanceled()
{
    return _canceled;
}

bool Database::removeBase(QString filename)
{
    emit toDebug(objectName(),
            QString("Удаляем старую базу '%1'.").arg(filename));

    removeConnection();
    QFile f(filename);
    if(f.exists())
    {
        if(f.remove())
        {
            emit toDebug(objectName(),
                         QString("База '%1' успешно удалена.").arg(filename));
            return true;
        }
        else
        {
            emit toDebug(objectName(),
                         QString("Невозможно удалить файл '%1'. Ошибка: %2")
                         .arg(filename)
                         .arg(f.errorString()));
            return false;
        }
    }
    else
    {
        emit toDebug(objectName(),
                QString("Невозможно удалить файл '%1'. Файл не существует.")
                     .arg(filename));
        return true;
    }
}

void Database::openBase(QString filename)
{
//    qDebug() << "Database openBase" << filename << this->thread()->currentThreadId();
    emit baseOpening();
    emit toDebug(objectName(),
                 QString("Открывается база данных '%1'.").arg(filename));
    if(filename!=baseName())
        removeConnection();
    setBaseName(filename);
    if(!_connected)
        createConnection();
    if(!_connected)
        return;
    createTable();
    createModel();
    emit baseOpened();
}

void Database::clear()
{
    if(_model!=nullptr)
    {
        _model->clear();
        emit selectedRows(0);
        delete _model;
        _model=nullptr;
    }
}

QSqlTableModel *Database::getModel()
{
    return _model;
}

void Database::createModel()
{
    clear();
    _model = new QSqlTableModel;
    _model->setTable(TableName);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();

    if(_model->lastError().isValid())
    {
        emit toDebug(objectName(),
                     "При создании модели возникла ошибка: "+_model->lastError().text());
    }
    else
    {
        emit toDebug(objectName(),
                     QString("Модель успешно создана и открыта."));
//        emit selectedRows(_model->rowCount());
    }
}

void Database::updateTableModel()
{
    emit toDebug(objectName(),
                 QString("Обновляется отображение модели"));
//    qDebug() << "_model" << _model << _model->rowCount();
    if(_model!=nullptr)
    {
        _model->select();
        emit selectedRows(_model->rowCount());
    }
    else
        emit selectedRows(0);
}

void Database::removeConnection()
{
//    qDebug() << "removeConnection()";
    if(!_connected)
        return;
    clear();
    QStringList cNames = QSqlDatabase::connectionNames();
    if(!cNames.isEmpty())
    {
        emit toDebug(objectName(),
                     QString("Удаляем соединение к базе '%1'.").arg(cNames.first()));
        QSqlDatabase::removeDatabase(cNames.first());
    }
    _connected=false;
}

void Database::createConnection()
{
    if(_connected)
        return;

    emit toDebug(objectName(),
                 QString("Cоздаем соединение к базе '%1'.").arg(baseName()));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(baseName());
    db.setUserName("user");
    db.setHostName("rt");
    db.setPassword("user");
    if (!db.open())
    {
        _connected=false;
        emit toDebug(objectName(),
                     QString("База данных '%1' не может быть открыта. Ошибка: %2")
                     .arg(baseName())
                     .arg(db.lastError().text()));
    }
    else
    {
        _connected=true;
        emit toDebug(objectName(),
                     QString("База данных '%1' открыта.").arg(baseName()));
    }
}

void Database::createTable()
{
    QStringList tables=QSqlDatabase::database().tables();
    if(tables.isEmpty() || !tables.contains(TableName))
    {
        QSqlQuery query;
        QString str =
                QString("CREATE TABLE IF NOT EXISTS %18 ( "
                        "'%1' INTEGER PRIMARY KEY NOT NULL, "
                        "'%2' TEXT, "
                        "'%3' TEXT, "
                        "'%4' TEXT, "
                        "'%5' TEXT, "
                        "'%6' TEXT, "
                        "'%7' TEXT, "
                        "'%8' TEXT, "
                        "'%9' TEXT, "
                        "'%10' TEXT, "
                        "'%11' TEXT, "
                        "'%12' TEXT, "
                        "'%13' TEXT, "
                        "'%14' TEXT, "
                        "'%15' TEXT, "
                        "'%16' TEXT, "
                        "'%17' TEXT "
                        ")"
                        )
                .arg(MapColumnNames[BUILD_ID])
                .arg(MapColumnNames[STREET])
                .arg(MapColumnNames[STREET_ID])
                .arg(MapColumnNames[KORP])
                .arg(MapColumnNames[BUILD])
                .arg(MapColumnNames[TYPE_OF_STREET])
                .arg(MapColumnNames[ADDITIONAL])
                .arg(MapColumnNames[TYPE_OF_CITY1])
                .arg(MapColumnNames[CITY1])
                .arg(MapColumnNames[TYPE_OF_CITY2])
                .arg(MapColumnNames[CITY2])
                .arg(MapColumnNames[DISTRICT])
                .arg(MapColumnNames[FSUBJ])
                .arg(MapColumnNames[RAW_ADDR])
                .arg(MapColumnNames[LITERA])
                .arg(MapColumnNames[CORRECT])
                .arg(MapColumnNames[TYPE_OF_FSUBJ])
                .arg(TableName);

        if(!query.exec(str))
            emit toDebug(objectName(),
                         QString("Невозможно создать таблицу '%1'. Ошибка: '%2'.")
                         .arg(TableName)
                         .arg(query.lastError().text()));
        else
            emit toDebug(objectName(),
                         QString("Таблица '%1' была создана.").arg(TableName));

        str =
        "PRAGMA synchronous = OFF;"
        "PRAGMA journal_mode = OFF;"
        "CREATE INDEX bid_indx ON base1(BUILD_ID);"
        "CREATE INDEX sid_indx ON base1(STREET_ID);"
        "CREATE INDEX tof_indx ON base1(TYPE_OF_FSUBJ);"
        "CREATE INDEX fs_indx ON base1(FSUBJ);"
        "CREATE INDEX d_indx ON base1(DISTRICT);"
        "CREATE INDEX toc1_indx ON base1(TYPE_OF_CITY1);"
        "CREATE INDEX c1_indx ON base1(CITY1);"
        "CREATE INDEX toc2_indx ON base1(TYPE_OF_CITY2);"
        "CREATE INDEX c2_indx ON base1(CITY2);"
        "CREATE INDEX tos_indx ON base1(TYPE_OF_STREET);"
        "CREATE INDEX s_indx ON base1(STREET);"
        "CREATE INDEX b_indx ON base1(BUILD);"
        "CREATE INDEX k_indx ON base1(KORP);"
        "CREATE INDEX l_indx ON base1(LITERA);"
        "CREATE INDEX c_indx ON base1(CORRECT);"
        "CREATE INDEX a_indx ON base1(ADDITIONAL);"
        "CREATE INDEX all_indx ON base1(STREET, STREET_ID, KORP, BUILD, BUILD_ID, ADDITIONAL, DISTRICT, FSUBJ, TYPE_OF_CITY1, CITY1, TYPE_OF_CITY2, CITY2, TYPE_OF_STREET, LITERA, CORRECT, TYPE_OF_FSUBJ);"
        "CREATE INDEX all2_indx ON base1(TYPE_OF_FSUBJ, FSUBJ, DISTRICT, TYPE_OF_CITY1, CITY1, TYPE_OF_CITY2, CITY2, TYPE_OF_STREET, STREET, BUILD, KORP, LITERA, CORRECT);";
        QStringList queries = str.split(";");
        foreach (QString q, queries) {
            if(q.isEmpty())
                continue;
            if(!query.exec(q))
                emit toDebug(objectName(),
                             QString("Невозможно выполнить '%1'. Ошибка: '%2'.")
                             .arg(q)
                             .arg(query.lastError().text()));
            else
                emit toDebug(objectName(),
                             QString("Выполнен запрос '%1'").arg(q));
        }
    }
}

void Database::insertListAddressWithCheck(const ListAddress &la)
{
    _bids.clear();
    _countNotInsertedRows=0;
    _canceled=false;
    ListAddress::const_iterator it=la.begin();
    QSqlDatabase::database().driver()->beginTransaction();
    for(;it!=la.end();it++)
    {
        if(_canceled){
//            qDebug() << "cancel";
            break;
        }

        if(it->getBuildId()!=0)
            insertAddressWithCheck(*it);
        else
            _countNotInsertedRows++;
    }
    QSqlDatabase::database().driver()->commitTransaction();
//    qDebug() << "BIDS: " << _bids.size() << "in memory:" << _bids.size()*8 << "bytes";
    if(_countNotInsertedRows>0)
        emit toDebug(objectName(),
                     QString("'%1' записей не были добавлены").arg(_countNotInsertedRows));
    _bids.clear();
    _countNotInsertedRows=0;
}

void Database::insertAddressWithCheck(const Address &a)
{
    if(_bids.contains(a.getBuildId()))
    {
        emit toDebug(objectName(),
                QString("Запись с BID='%1' не может быть добавлена, т.к. такая запись уже существует в БД")
                .arg(a.getBuildId()));

        QSqlQuery query;
        QString str = QString("SELECT STREET_ID, BUILD_ID, RAW "
                              "FROM %1 "
                              "WHERE BUILD_ID = '%2'").arg(TableName).arg(a.getBuildId());
        if (!query.exec(str))
        {
            emit toDebug(objectName(),
                    QString("Невозможно выполнить операцию выборки '%1' в БД. Ошибка: %2")
                         .arg(str)
                         .arg(query.lastError().text()));
        }
        QSqlRecord rec = query.record();
        Address tmp;
        if(query.next())
        {
            tmp.setRawAddress( query.value(rec.indexOf("RAW")).toString() );
            tmp.setBuildId( query.value(rec.indexOf("BUILD_ID")).toULongLong() );
            tmp.setStreetId( query.value(rec.indexOf("STREET_ID")).toULongLong() );
        }
        emit toDebug(objectName(),
                QString("В базе находится следующая строка '%1'")
                .arg(tmp.toCsv()));
        _countNotInsertedRows++;
        return;
    }
    _bids.insert(a.getBuildId());
    QSqlQuery query;

    if (!query.exec(a.toInsertSqlQuery()))
    {
        emit toDebug(objectName(),
                QString("Невозможно выполнить операцию вставки строки '%1' в БД. Ошибка: %2")
                     .arg(a.toCsv())
                     .arg(query.lastError().text()));
        //        assert(0);
        _countNotInsertedRows++;
    }
}

void Database::selectAddress(Address a)
{
    if(_model==nullptr)
        return;
    QString filter;
    if(a.getTypeOfFSubj()!=INCORRECT_SUBJ)
        filter+=(filter.isEmpty()?"":" AND ") + QString("TYPE_OF_FSUBJ = '%1'").arg(a.getTypeOfFSubjInString());
    else
        filter+=(filter.isEmpty()?"":" AND ") + QString("TYPE_OF_FSUBJ = 'inc'");
    if(a.getFsubj()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("FSUBJ = '%1'").arg(a.getFsubj());
    if(a.getDistrict()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("DISTRICT = '%1'").arg(a.getDistrict());
    if(a.getTypeOfCity1()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("TYPE_OF_CITY1 = '%1'").arg(a.getTypeOfCity1());
    if(a.getCity1()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("CITY1 = '%1'").arg(a.getCity1());
    if(a.getTypeOfCity2()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("TYPE_OF_CITY2 = '%1'").arg(a.getTypeOfCity2());
    if(a.getCity2()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("CITY2 = '%1'").arg(a.getCity2());
    if(a.getTypeOfStreet()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("TYPE_OF_STREET = '%1'").arg(a.getTypeOfStreet());
    if(a.getStreet()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("STREET = '%1'").arg(a.getStreet());
    if(a.getBuild()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("BUILD = '%1'").arg(a.getBuild());
    if(a.getKorp()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("KORP = '%1'").arg(a.getKorp());
    if(a.getLitera()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("LITERA = '%1'").arg(a.getLitera());
    if(a.getAdditional()!="*")
       filter+=(filter.isEmpty()?"":" AND ") + QString("ADDITIONAL = '%1'").arg(a.getAdditional());
    if(a.getBuildId()!=0)
       filter+=(filter.isEmpty()?"":" AND ") + QString("BUILD_ID = '%1'").arg(a.getBuildId());
    if(a.getStreetId()!=0)
       filter+=(filter.isEmpty()?"":" AND ") + QString("STREET_ID = '%1'").arg(a.getStreetId());
    if(a.isCorrect())
        filter+=(filter.isEmpty()?"":" AND ") + QString("CORRECT = '1'");
    else
        filter+=(filter.isEmpty()?"":" AND ") + QString("CORRECT = '0'");

    emit toDebug(objectName(),
                 QString("Устанавливаем фильтр: %1").arg(filter));
    _model->setFilter(filter);
    emit toDebug(objectName(),
                 QString("Фильтр установлен: %1").arg(_model->filter()));

    emit toDebug(objectName(),
                 QString("Выполняем select"));
    _model->select();

    emit toDebug(objectName(),
                 QString("Select выполнен. Найдено строк: %1").arg(_model->rowCount()));

    emit selectedRows(_model->rowCount());
}

void Database::selectAddress(QString sheet, int nRow, Address a)
{
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
//    qDebug().noquote() << "Database::selectAddress" << str;
    QSqlQuery query;
    if (!query.exec(str))
    {
        qDebug() << "Unable to execute query - exiting"
                 << endl
                 << query.lastError().text();
        return;
    }

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
}

void Database::dropTable()
{
//    qDebug().noquote() << "Database dropTable" << QThread::currentThreadId();
    QSqlQuery query(
                QString("DROP TABLE IF EXISTS %1;")
                .arg(TableName)
                );

    if(!query.exec())
    {
//        qDebug().noquote() << "dropTable error" << QThread::currentThreadId();

        emit toDebug(objectName(),
                "Unable to drop a table:\n"+query.lastError().text());
    }
    else
    {
//        qDebug().noquote() << "dropTable success" << QThread::currentThreadId();

        emit toDebug(objectName(),
                "Success drop a table");
    }
}

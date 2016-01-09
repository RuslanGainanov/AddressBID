#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent),
    _model(nullptr),
    _connected(false)
{
//    _model = new QSqlTableModel(this);
//    _model->setTable(TableName);
//    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
//    if(_model->lastError().isValid())
//        emit toDebug(objectName(),
//                     "openModel:"+_model->lastError().text());
}

Database::~Database()
{
    if(_model!=nullptr)
        delete _model;
}

void Database::setBaseName(QString name)
{
    _baseName = name;
}

QString Database::baseName()
{
    return _baseName;
}

ListAddress Database::search(QString sheetName, ListAddress addr)
{
    qDebug() << "Database::search" << this->thread()->currentThreadId()
             << sheetName << addr.size();
    for(int i=0; i<addr.size(); i++)
    {
        selectAddress(addr[i]);
    }
    return addr;
}

void Database::removeBase(QString filename)
{
    qDebug() << "Database removeBase" << filename << this->thread()->currentThreadId();
    removeConnection();
    if(QFile::exists(filename))
    {
        if(QFile::remove(filename))
            emit toDebug(objectName(),
                    QString("%1 remove success").arg(filename));
        else
            emit toDebug(objectName(),
                    QString("%1 remove error").arg(filename));
    }
    else
    {
        emit toDebug(objectName(),
                QString("%1 cannot remove. File is not exists").arg(filename));
    }
}

void Database::openBase(QString filename)
{
    qDebug() << "Database openBase" << filename << this->thread()->currentThreadId();
    if(filename!=_baseName)
        removeConnection();
    setBaseName(filename);
    if(!_connected)
        createConnection();
    if(!_connected)
        return;
    createTable();
    openTableToModel();
    emit countRows(_model->rowCount());
    emit baseOpened();
}

void Database::clear()
{
}

QSqlTableModel *Database::getModel()
{
    return _model;
}

void Database::openTableToModel()
{
    if(_model!=nullptr)
        delete _model;
    _model = new QSqlTableModel(this);
    _model->setTable(TableName);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();

    if(_model->lastError().isValid())
        emit toDebug(objectName(),
                     "openModel:"+_model->lastError().text());
}

void Database::updateTableModel()
{
    _model->select();
}

void Database::removeConnection()
{
    if(!_connected)
        return;

    if(_model!=nullptr)
    {
        delete _model;
        _model=nullptr;
    }
    QSqlDatabase::removeDatabase(_baseName);
    _connected=false;
}

void Database::createConnection()
{
    if(_connected)
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(_baseName);
    db.setUserName("user");
    db.setHostName("rt");
    db.setPassword("user");
    QString str;
    if (!db.open())
    {
        _connected=false;
        str+="Cannot open database:" + db.lastError().text();
        emit toDebug(objectName(),
                     str);
    }
    else
    {
        _connected=true;
        str += "Success open base:" + _baseName;
        emit toDebug(objectName(),
                     str);
    }
}

void Database::createTable()
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
                    ");")
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
                "Unable to create a table:\r\n"+query.lastError().text());
    else
        emit toDebug(objectName(),
                "Success create a table");
}

void Database::insertListAddressWithCheck(ListAddress &la)
{
    ListAddress::iterator it=la.begin();
    for(;it!=la.end();it++)
    {
        insertAddressWithCheck(*it);
    }
}

void Database::insertAddressWithCheck(Address &a)
{
    if(_bids.contains(a.getBuildId()))
    {
        emit toDebug(objectName(),
                "Database already contains this entry (BID):\r\n"
                +QString::number(a.getBuildId()));
        return;
    }
    _bids.insert(a.getBuildId());
    QSqlQuery query;
    if (!query.exec(a.toInsertSqlQuery()))
    {
        emit toDebug(objectName(),
                "Unable to make insert operation:\r\n"
                +query.lastError().text());
        //        assert(0);
    }
}

void Database::selectAddress(Address &a)
{
    int n = qrand();
    if(a.getBuildId()!=0 && a.getStreetId()!=0)
        return;
    if(           n%5==0
               || n%5==1
               || n%5==2
               || n%5==3
               /*|| n%5==4*/ )
    {
        a.setBuildId( 100000+qrand()%100000 );
        a.setStreetId( 100000+qrand()%100000 );
        return;
    }
    return;

    QSqlQuery query;
    if (!query.exec(QString("SELECT STREET_ID, BUILD_ID "
                    "FROM %6"
                    "WHERE STREET = '%1'"
                    "  AND TYPE_OF_STREET = '%2'"
                    "  AND CITY1 = '%3'"
                    "  AND BUILD = '%4'"
                    "  AND KORP = '%5';")
                    .arg(a.getStreet())
                    .arg(a.getTypeOfStreet())
                    .arg(a.getCity1())
                    .arg(a.getBuild())
                    .arg(a.getKorp()))) {
        qDebug() << "Unable to execute query - exiting"
                 << endl
                 << query.lastError().text();
        return;
    }

    //Reading of the data
    QSqlRecord rec     = query.record();
    while (query.next()) {
        a.setBuildId( query.value(rec.indexOf("BUILD_ID")).toULongLong() );
        a.setStreetId( query.value(rec.indexOf("STREET_ID")).toULongLong() );
    }
}

void Database::dropTable()
{
    qDebug().noquote() << "Database dropTable" << QThread::currentThreadId();
    QSqlQuery query(
                QString("DROP TABLE IF EXISTS %1;")
                .arg(TableName)
                );

    if(!query.exec())
    {
        qDebug().noquote() << "dropTable error" << QThread::currentThreadId();

        toDebug(objectName(),
                "Unable to drop a table:\n"+query.lastError().text());
    }
    else
    {
        qDebug().noquote() << "dropTable success" << QThread::currentThreadId();

        toDebug(objectName(),
                "Success drop a table");
    }
}

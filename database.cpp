#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent),
    _thread(nullptr),
    _connected(false),
    _countParsedRows(0),
    _model(nullptr)
{
    _thread = new QThread;
}

Database::~Database()
{
    if(_thread)
    {
        _thread->quit();
        _thread->wait();
        delete _thread;
    }
    if(_model!=nullptr)
        delete _model;
}

void Database::setBaseName(QString name)
{
    _baseName = name;
}

void Database::openOldBase()
{
    createConnection();
    openTableToModel();
    emit countRows(_model->rowCount());
    emit baseOpened();
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

void Database::openBase(QString filename)
{
    qDebug() << "Database openBase" << this->thread()->currentThreadId();
    if(_thread->isRunning())
        return;

    if(!_connected)
        createConnection();
    if(!_connected)
        return;

    dropTable();
    createTable();

    _csvWorker = new CsvWorker;
    _csvWorker->setFileName(filename);
    _csvWorker->setMaxCountRead((int)MAX_OPEN_ROWS);
    _csvWorker->moveToThread(_thread);
    connect(_thread, SIGNAL(started()),
            _csvWorker, SLOT(process()));
    connect(_csvWorker, SIGNAL(countRows(int)),
            this, SLOT(onCountRows(int)));
    connect(_csvWorker, SIGNAL(rowReaded(int,QStringList)),
            this, SLOT(onReadRow(int,QStringList)));
    connect(_csvWorker, SIGNAL(rowsReaded(int)),
            this, SLOT(onReadRows(int)));
    connect(_csvWorker, SIGNAL(finished()),
            this, SLOT(onFinishCsvWorker()));

    _parser = new Parser;
    _parser->setTypeOfRow(BASE_TYPE);
    _parser->moveToThread(_thread);
    connect(_csvWorker, SIGNAL(headReaded(QStringList)),
            _parser, SLOT(onReadHeadBase(QStringList)));
    connect(_csvWorker, SIGNAL(rowReaded(int,QStringList)),
            _parser, SLOT(onReadRow(int,QStringList)));
    connect(_parser, SIGNAL(rowParsed(int, Address)),
            this, SLOT(onParseRow(int, Address)));

    connect(_thread, SIGNAL(finished()),
            _parser, SLOT(deleteLater()));
    connect(_thread, SIGNAL(finished()),
            _csvWorker, SLOT(deleteLater()));
    _thread->start();

    emit workingWithOpenBase();
}

void Database::clear()
{
}

void Database::onCountRows(int count)
{
    qDebug() << "Database onCountRow" << count;
    emit countRows(count);
}

void Database::onReadRows(int rows)
{
    qDebug() << "Database onReadRows" << rows;
    emit readedRows(rows);
}

void Database::onReadRow(int rowNumber, QStringList row)
{
    Q_UNUSED(row);
//    qDebug() << "Database onReadRow" /*<< row*/ << rowNumber;
    emit rowReaded(rowNumber);
}

void Database::onParseRow(int rowNumber, Address a)
{
//    Q_UNUSED(addr);
//    qDebug() << "Database onParseRow" << rowNumber /*<< addr.toDebug()*/;
    _countParsedRows++;
    _mapSet[ENAME].insert(a.getEname());
    _mapSet[CITY].insert(a.getCity());
    _mapSet[DISTRICT].insert(a.getDistrict());
    _mapSet[FSUBJ].insert(a.getFsubj());
    _mapSet[STREET].insert(a.getStreet());
    insertAddress(rowNumber, a);
//    emit toDebug("row"+QString::number(rowNumber)+"\nstr:"+a.getStreet()+"\nb:"+a.getBuild()+"\nk:"+a.getKorp());
    emit rowParsed(rowNumber);
}

void Database::onFinishParser()
{
}

void Database::onFinishCsvWorker()
{
    _thread->quit();
    _thread->wait();
    QString mes;
    mes+="Count rows:"+QString::number(_countParsedRows)+"\r\n";
    mes+="Count city:"+QString::number(_mapSet[CITY].size())+"\r\n";
    mes+="Count district:"+QString::number(_mapSet[DISTRICT].size())+"\r\n";
    mes+="Count ename:"+QString::number(_mapSet[ENAME].size())+"\r\n";
    mes+="Count federal subject:"+QString::number(_mapSet[FSUBJ].size())+"\r\n";
    mes+="Count street:"+QString::number(_mapSet[STREET].size())+"\r\n";
    emit messageReady(mes);
    openTableToModel();
    emit baseOpened();
}

QSqlTableModel *Database::getModel()
{
    return _model;
}

void Database::openTableToModel()
{
    if(_model!=nullptr)
        delete _model;
    _model = new QSqlTableModel(this, _db);
    _model->setTable("base");
//    _model->setTable("BASE1");
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();

    if(_model->lastError().isValid())
        emit toDebug("Database openModel:"+_model->lastError().text());
}

void Database::createConnection()
{
    if(_connected)
        return;

    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(_baseName);
    _db.setUserName("user");
    _db.setHostName("rt");
    _db.setPassword("user");
    QString str;
    if (!_db.open())
    {
        _connected=false;
        str+="Cannot open database:" + _db.lastError().text();
        emit toDebug(str);
    }
    else
    {
        _connected=true;
        str += "Success open base:" + _baseName;
        emit toDebug(str);
    }
}

void Database::createTable()
{
    QSqlQuery query;
    QString str =
            QString("CREATE TABLE IF NOT EXISTS base ( "
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
                    "'%12' TEXT "
                    ");")
            .arg("ID") //rowNumber
            .arg(MapColumnNames[STREET])
            .arg(MapColumnNames[STREET_ID])
            .arg(MapColumnNames[KORP])
            .arg(MapColumnNames[BUILD])
            .arg(MapColumnNames[BUILD_ID])
            .arg(MapColumnNames[ENAME])
            .arg(MapColumnNames[ADDITIONAL])
            .arg(MapColumnNames[CITY])
            .arg(MapColumnNames[DISTRICT])
            .arg(MapColumnNames[FSUBJ])
            .arg(MapColumnNames[RAW_ADDR]);

    if(!query.exec(str))
        toDebug("Unable to create a table:\r\n"+query.lastError().text());
    else
        toDebug("Success create a table");

//    if(_model==nullptr)
//    {
//        _model = new QSqlTableModel;
//        _model->setTable("base");
//        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
//        _model->select();
//    }
}

void Database::insertAddress(int row, const Address &a)
{
    QSqlQuery query;
    QString strF =
          "INSERT INTO  base ('ID', '%1', '%2', '%3', '%4', '%5',"
          " '%6', '%7', '%8', '%9', '%10', '%11') "
          "VALUES('%12', '%13', '%14', '%15', '%16',"
          " '%17', '%18', '%19', '%20', '%21', '%22', '%23');";

    QString str =
            strF
            .arg(MapColumnNames[STREET])
            .arg(MapColumnNames[STREET_ID])
            .arg(MapColumnNames[KORP])
            .arg(MapColumnNames[BUILD])
            .arg(MapColumnNames[BUILD_ID])
            .arg(MapColumnNames[ENAME])
            .arg(MapColumnNames[ADDITIONAL])
            .arg(MapColumnNames[CITY])
            .arg(MapColumnNames[DISTRICT])
            .arg(MapColumnNames[FSUBJ])
            .arg(MapColumnNames[RAW_ADDR])
            .arg(QString::number(row))
            .arg(a.getStreet())
            .arg(QString::number(a.getStreetId()))
            .arg(a.getKorp())
            .arg(a.getBuild())
            .arg(QString::number(a.getBuildId()))
            .arg(a.getEname())
            .arg(a.getAdditional())
            .arg(a.getCity())
            .arg(a.getDistrict())
            .arg(a.getFsubj())
            .arg(a.getRawAddress().join(';'));
    if (!query.exec(str))
    {
        toDebug("Unable to make insert opeation:"
                +a.toDebug(RAW).join(';')
                +":\r\n"+query.lastError().text());
    }
//    else
//        toDebug("Success make insert opeation");

}

void Database::selectAddress(Address &a)
{
    int n = qrand();
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
                    "FROM BASE"
                    "WHERE STREET = '%1'"
                    "  AND TYPE_OF_STREET = '%2.'"
                    "  AND CITY1 = '%3'"
                    "  AND BUILD = '%4'"
                    "  AND KORP = '%5';")
                    .arg(a.getStreet())
                    .arg(a.getTypeOfStreet())
                    .arg(a.getCity())
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
    QSqlQuery query;
    QString str =
            "DROP TABLE IF EXISTS base;";

    if(!query.exec(str))
        toDebug("Unable to drop a table:\n"+query.lastError().text());
    else
        toDebug("Success drop a table");

//    if(_model!=nullptr)
//    {
//        delete _model;
//        _model=nullptr;
//    }
}

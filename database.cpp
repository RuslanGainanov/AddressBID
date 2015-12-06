#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent),
    _thread(nullptr),
    _connected(false),
    _countParsedRows(0)
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
}

void Database::setBaseName(QString name)
{
    _baseName = name;
}

void Database::openOldBase(QString name)
{
    _baseName = name;
    createConnection();
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
    _parser->setTypeOfRow(BASE);
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
    qDebug() << "Database onReadRow" /*<< row*/ << rowNumber;
    emit rowReaded(rowNumber);
}

void Database::onParseRow(int rowNumber, Address a)
{
//    Q_UNUSED(addr);
    qDebug() << "Database onParseRow" << rowNumber /*<< addr.toDebug()*/;
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
    emit baseOpened();
    QString mes;
    mes+="Count rows:"+QString::number(_countParsedRows)+"\n";
    mes+="Count city:"+QString::number(_mapSet[CITY].size())+"\n";
    mes+="Count district:"+QString::number(_mapSet[DISTRICT].size())+"\n";
    mes+="Count ename:"+QString::number(_mapSet[ENAME].size())+"\n";
    mes+="Count federal subject:"+QString::number(_mapSet[FSUBJ].size())+"\n";
    mes+="Count street:"+QString::number(_mapSet[STREET].size())+"\n";
    emit messageReady(mes);

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
            QString("CREATE TABLE base ( "
            "%1 INTEGER PRIMARY KEY NOT NULL, "
            "%2 VARCHAR(15), "
            "%3 VARCHAR(15), "
            "%4 VARCHAR(255) "
            ");")
            .arg("row")
            .arg("sid")
            .arg("bid")
            .arg("raw");

    if(!query.exec(str))
        toDebug("Unable to create a table");
    else
        toDebug("Success create a table");

}

void Database::insertAddress(int row, const Address &a)
{
    QSqlQuery query;
    QString strF =
          "INSERT INTO  base (row, sid, bid, raw) "
          "VALUES(%1, '%2', '%3', '%4');";

    QString str = strF.arg(QString::number(row))
              .arg(QString::number(a.getStreetId()))
              .arg(QString::number(a.getBuildId()))
              .arg(a.getRawAddress().join(' '));
    if (!query.exec(str))
        toDebug("Unable to make insert opeation");
    else
        toDebug("Success make insert opeation");

}

void Database::dropTable()
{
    QSqlQuery query;
    QString str =
            "DROP TABLE base;";

    if(!query.exec(str))
        toDebug("Unable to drop a table");
    else
        toDebug("Success drop a table");
}

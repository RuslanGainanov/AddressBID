#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent),
    _thread(nullptr)
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


void Database::openBase(QString filename)
{
    qDebug() << "Database openBase" << this->thread()->currentThreadId();
    if(_thread->isRunning())
        return;

    _csvWorker = new CsvWorker;
//    _csvWorker.reset(new CsvWorker);
    _csvWorker->setFileName(filename);
//    _csvWorker->setMaxCountRead(10);
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
//    _parser.reset(new Parser);
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

void Database::onParseRow(int rowNumber, Address addr)
{
    Q_UNUSED(addr);
    qDebug() << "Database onParseRow" << rowNumber /*<< addr.toDebug()*/;
    emit rowParsed(rowNumber);
}

void Database::onFinishParser()
{
//    delete _parser;
}

void Database::onFinishCsvWorker()
{
//    delete _csvWorker;
    _thread->quit();
    _thread->wait();
    emit baseOpened();
}

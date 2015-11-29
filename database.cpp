#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent)
{
    _vect = new QVector<VectorOfString>;
    _thread = new QThread;
    _countThreadParser=0;
}

Database::~Database()
{
    delete _vect;
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
    _csvWorker->setFileName(filename);
//    _csvWorker->setMaxCountRead(10);
    _csvWorker->moveToThread(_thread);
    connect(_thread, SIGNAL(started()),
            _csvWorker, SLOT(process()));
//    connect(_csvWorker, SIGNAL(finished()),
//            _thread, SLOT(quit()));
    connect(_csvWorker, SIGNAL(finished()),
            _csvWorker, SLOT(deleteLater()));
    connect(_csvWorker, SIGNAL(countRows(int)),
            this, SLOT(onCountRows(int)));
    connect(_csvWorker, SIGNAL(rowReaded(int,QStringList)),
            this, SLOT(onReadRow(int,QStringList)));
    connect(_csvWorker, SIGNAL(rowsReaded(int)),
            this, SLOT(onReadRows(int)));

    Parser *parser = new Parser;
    parser->setTypeOfRow(BASE);
    parser->moveToThread(_thread);
    connect(_csvWorker, SIGNAL(rowReaded(int,QStringList)),
            parser, SLOT(onReadRow(int,QStringList)));
    connect(_csvWorker, SIGNAL(rowReaded(int,QStringList)),
            parser, SLOT(process()));
    connect(_csvWorker, SIGNAL(headReaded(QStringList)),
            parser, SLOT(onReadHeadBase(QStringList)));
    connect(parser, SIGNAL(rowParsed(int, Address)),
            this, SLOT(onParseRow(int, Address)));
    connect(parser, SIGNAL(finished()),
            parser, SLOT(deleteLater()));

    _thread->start();

}

void Database::clear()
{
    _vect->clear();
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

//    QThread *threadParser = new QThread;
//    Parser *parser = new Parser;
//    parser->setTypeOfRow(BASE);
//    parser->setHeadBase(_mapHead);
//    parser->onReadRow(rowNumber, row);
//    parser->moveToThread(threadParser);
//    connect(threadParser, SIGNAL(started()),
//            parser, SLOT(process()));
//    connect(parser, SIGNAL(finished()),
//            threadParser, SLOT(quit()));
//    connect(parser, SIGNAL(finished()),
//            parser, SLOT(deleteLater()));
//    connect(parser, SIGNAL(rowParsed(int, Address)),
//            this, SLOT(onParseRow(int, Address)));
//    connect(threadParser, SIGNAL(finished()),
//            threadParser, SLOT(deleteLater()));
//    threadParser->start();
}

void Database::onParseRow(int rowNumber, Address addr)
{
    Q_UNUSED(addr);
    qDebug() << "Database onParseRow" << rowNumber /*<< addr.toDebug()*/;
    emit rowParsed(rowNumber);
}

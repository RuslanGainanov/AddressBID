#include "csvworker.h"

CsvWorker::CsvWorker(QObject *parent) :
    QObject(parent),
    _maxCount(0)
{
    qDebug() << "CsvWorker constructor" << this->thread()->currentThreadId();
}


CsvWorker::~CsvWorker()
{
    qDebug() << "CsvWorker destructor" << this->thread()->currentThreadId();
}


void CsvWorker::setFileName(QString fname)
{
    _filename = fname;
}

void CsvWorker::setMaxCountRead(int maxCount)
{
    if(maxCount>0)
        _maxCount=maxCount;
}

void CsvWorker::process()
{
    qDebug() << "CsvWorker process" << this->thread()->currentThreadId();

    QFile file1(_filename);
    if (!file1.open(QIODevice::ReadOnly))
    {
        qDebug() << "Ошибка открытия для чтения";
        return;
    }

    int approxRows = file1.size()/90; // 90байт - столько примерно занимает одна строка в файле
    if(_maxCount>0)
        emit countRows(_maxCount);
    else
        emit countRows(approxRows);

    QTextStream in(&file1);
    QTextCodec *defaultTextCodec = QTextCodec::codecForName("Windows-1251");
    QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);
    int rows=0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QString readedStr = decoder->toUnicode(line.toLocal8Bit());

        QStringList row = readedStr.split(";");
        if(!row.isEmpty())
        {
            for(int i=0; i<row.size(); i++)
            {
                row[i].remove("\"");
                row[i] = row.at(i).trimmed();
            }

            if(rows==0)
                emit headReaded(row);
            else
                emit rowReaded(rows-1, row);
            rows++;
        }
        //оставливаем обработку если получено нужное количество строк
        if(_maxCount>0 && rows >= _maxCount)
            break;
    }
    delete decoder;
    file1.close();
    emit rowsReaded(rows);
    emit finished();
}

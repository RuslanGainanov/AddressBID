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
    QTextCodec *defaultTextCodec = QTextCodec::codecForName("Windows-1251");
    QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);
    QString *readedFileStr = new QString (decoder->toUnicode(file1.readAll()));
    delete decoder;
    file1.close();

    QStringList *rowList = new QStringList (readedFileStr->split("\n"));
    if(_maxCount>0)
        emit countRows(_maxCount);
    else
    {
        emit countRows(rowList->size()-1); //-1 - 1 строка - шапка
    }
    delete readedFileStr;

    //получение шапки
    QStringList head = rowList->at(0).split(";");
    for(int i=0; i<head.size(); i++)
    {
        head[i].remove("\"");
        head[i] = head.at(i).trimmed();
    }
    emit headReaded(head);
    //шапка прочитана!

    //чтение данных
    int rows=0;
    for(int i=1; i<rowList->size(); i++)
    {
//        qDebug() << rowList.at(i);
        QStringList row = rowList->at(i).split(";");
        if(!row.isEmpty())
        {
            emit rowReaded(i-1, row);
            rows++;

            //оставливаем обработку если получено нужное количество строк
            if(_maxCount>0 && rows >= _maxCount)
            {
                break;
            }
        }
    }
    delete rowList;
    //чтение данных окончено!
    emit rowsReaded(rows-1);
    emit finished();
}

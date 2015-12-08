#include "xlsworker.h"

XlsWorker::XlsWorker(QObject *parent) :
    QObject(parent),
    _maxCount(0)
{
    qDebug() << "XlsWorker constructor" << this->thread()->currentThreadId();
}

XlsWorker::~XlsWorker()
{
    qDebug() << "XlsWorker destructor" << this->thread()->currentThreadId();
}

void XlsWorker::setFileName(QString fname)
{
    _filename = fname;
}

void XlsWorker::setMaxCountRead(int maxCount)
{
    if(maxCount>0)
        _maxCount=maxCount;
}

void XlsWorker::debugError(int i, QString s1, QString s2, QString s3)
{
#if 1
    emit toDebug(objectName(), QString::number(i) + s1 + s2 + s3);
#else
    Q_UNUSED(i);
    Q_UNUSED(s1);
    Q_UNUSED(s2);
    Q_UNUSED(s3);
#endif
}

void XlsWorker::process()
{
    qDebug() << "XlsWorker process" << this->thread()->currentThreadId();

    QAxObject *excel = new QAxObject("Excel.Application"); // получаем указатель на Excel
    if(excel==NULL)
    {
        QString error="Cannot get Excel.Application";
        toDebug(objectName(), error);
        emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
        emit finished();
        return;
    }
    connect(excel,SIGNAL(exception(int, QString, QString, QString)),
            this,SLOT(debugError(int,QString,QString,QString)));

    QAxObject *workbooks = excel->querySubObject("Workbooks");
    if(workbooks==NULL)
    {
        QString error="Cannot query Workbooks";
        toDebug(objectName(), error);
        emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
        emit finished();
        return;
    }
    connect(workbooks, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));

    // на директорию, откуда грузить книгу
    QAxObject *workbook = workbooks->querySubObject(
                "Open(const QString&)",
                _filename);
    if(workbook==NULL)
    {
        QString error=
                QString("Cannot query workbook.Open(const %1)")
                .arg(_filename);
        toDebug(objectName(), error);
        emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
        emit finished();
        return;
    }
    connect(workbook, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));

    QAxObject *sheets = workbook->querySubObject("Sheets");
    if(sheets==NULL)
    {
        QString error="Cannot query Sheets";
        toDebug(objectName(), error);
        emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
        emit finished();
        return;
    }
    connect(sheets, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));

    int count = sheets->dynamicCall("Count()").toInt(); //получаем кол-во листов
    QStringList sheetNames;
    //читаем имена листов
    for (int i=1; i<=count; i++)
    {
        QAxObject *sheet1 = sheets->querySubObject("Item(int)", i);
        if(sheet1==NULL)
        {
            QString error="Cannot query Item(int)"+QString::number(i);
            toDebug(objectName(), error);
            emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
            emit finished();
            return;
        }
        sheetNames.append( sheet1->dynamicCall("Name()").toString() );
        sheet1->clear();
        delete sheet1;
        sheet1 = NULL;
    }

    // проходим по всем листам документа
    int sheetNumber=0;
    foreach (QString sheetName, sheetNames)
    {
//        if(sheetName.isEmpty())
//            continue;
        QAxObject *sheet = NULL;
        sheet = sheets->querySubObject(
                    "Item(const QVariant&)", QVariant(sheetName));
        if(sheet==NULL)
        {
            QString error=
                    QString("Cannot query Item(const %1)")
                    .arg(sheetName);
            toDebug(objectName(), error);
            emit errorOccured(objectName(), XLS_WORKER_ERROR, error);
            emit finished();
            return;
        }
        connect(sheet, SIGNAL(exception(int,QString,QString,QString)),
                this, SLOT(debugError(int,QString,QString,QString)));

        QAxObject *usedRange = sheet->querySubObject("UsedRange");
        QAxObject *usedRows = usedRange->querySubObject("Rows");
        QAxObject *usedCols = usedRange->querySubObject("Columns");
        int rows = usedRows->property("Count").toInt();
        int cols = usedCols->property("Count").toInt();

        //если на данном листе всего 1 строка (или меньше), т.е. данный лист пуст
        if(rows<=1)
        {
            // освобождение памяти
            usedRange->clear();
            delete usedRange;
            usedRange = NULL;

        //    delete usedRows;
        //    usedRows = NULL;

        //    delete usedCols;
        //    usedCols = NULL;

            sheet->clear();
            delete sheet;
            sheet = NULL;

            sheetNumber++;
            continue;
        }
        _sheetNames.insert(sheetNumber, sheetName);

        emit toDebug(objectName(),
                QString("На листе %1: %2 строк, %3 столбцов")
                .arg(sheetName)
                .arg(QString::number(rows))
                .arg(QString::number(cols)));
        emit countRows(sheetNumber, rows);

        //чтение данных
        for(int row=1; row<=rows; row++)
        {
            QStringList strListRow;
            for(int col=1; col<=cols; col++)
            {
                QAxObject* cell =
                        sheet->querySubObject("Cells(QVariant,QVariant)", row, col);
                QString result = cell->property("Value").toString();
                strListRow.append(result);
                delete cell;
            }
            if(row==1)
                emit headReaded(sheetNumber, strListRow); //шапка прочитана (1 строки у таблиц)
            else
                emit rowReaded(sheetNumber, row-2, strListRow); //строка прочитана
                                            //(-2 - 1-я строка шапка + нумерация с 1
                                            //- приводим к нумерации к 0)
            //оставливаем обработку если получено нужное количество строк
            if(_maxCount>0 && row-1 >= _maxCount)
                break;
        }
        emit rowsReaded(sheetNumber, rows); //прочитано строк на данном листе
        sheetNumber++;

        // освобождение памяти
        usedRange->clear();
        delete usedRange;
        usedRange = NULL;

    //    delete usedRows;
    //    usedRows = NULL;

    //    delete usedCols;
    //    usedCols = NULL;

        sheet->clear();
        delete sheet;
        sheet = NULL;
    }//end foreach _sheetNames
    emit sheetsReaded(_sheetNames); //список листов прочитан в документе

    sheets->clear();
    delete sheets;
    sheets = NULL;

    workbook->clear();
    delete workbook;
    workbook = NULL;

    workbooks->dynamicCall("Close()");
    workbooks->clear();
    delete workbooks;
    workbooks = NULL;

    excel->dynamicCall("Quit()");
    delete excel;
    excel = NULL;

    emit finished();
}

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

QString XlsWorker::getListName(int numberList)
{
    if(numberList>0&&numberList<_listNames.size())
        return _listNames.at(numberList);
    else
        return QString();
}

void XlsWorker::process()
{
    qDebug() << "XlsWorker process" << this->thread()->currentThreadId();

    QAxObject *excel = new QAxObject("Excel.Application",this); // получаем указатель на Excel
    if(excel==NULL)
    {
        toDebug(objectName()+"Cannot get Excel.Application");
        return;
    }
    QAxObject *workbooks = excel->querySubObject("Workbooks");
    if(workbooks==NULL)
    {
        qDebug() << "false";
//        return false;
    }
    // на директорию, откуда грузить книгу
    QAxObject *workbook = workbooks->querySubObject(
                "Open(const QString&)",
                str);

//    QAxObject *workbook = workbooks->querySubObject("Add()");
//    if(workbook==NULL)
//    {
//        qDebug() << "false";
////        return false;
//    }
    QAxObject *sheets = workbook->querySubObject("Sheets");
    if(sheets==NULL)
    {
        qDebug() << "false";
//        return false;
    }

    int count = sheets->dynamicCall("Count()").toInt();
    QString firstSheetName;
    for (int i=1; i<=count; i++)
    {
        QAxObject *sheet1 = sheets->querySubObject("Item(int)", i);
        if(sheet1==NULL)
        {
            qDebug() << "false";
//            return false;
        }
        firstSheetName = sheet1->dynamicCall("Name()").toString();
        sheet1->clear();
        delete sheet1;
        sheet1 = NULL;
        break;
    }

    QAxObject *sheet = NULL;
    if(!firstSheetName.isEmpty())
        sheet = sheets->querySubObject(
                    "Item(const QVariant&)", QVariant(firstSheetName));
    if(sheet==NULL)
    {
        qDebug() << "false";
//        return false;
    }

    connect(excel,SIGNAL(exception(int, QString, QString, QString)),
                     this,SLOT(debugError(int,QString,QString,QString)));
    connect(workbooks, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));
    connect(workbook, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));
    connect(sheets, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));
    connect(sheet, SIGNAL(exception(int,QString,QString,QString)),
            this, SLOT(debugError(int,QString,QString,QString)));

    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    QAxObject *usedRows = usedRange->querySubObject("Rows");
    QAxObject *usedCols = usedRange->querySubObject("Columns");
    int rows = usedRows->property("Count").toInt();
    int cols = usedCols->property("Count").toInt();
    qDebug() << "Количество строк и столбцов в файле: " << rows << cols;

    //заполняем таблицу
    QTableWidget *tbl = ui->tableWidgetIn;
    QTableWidgetItem *ptwi = 0;
    tbl->setColumnCount(cols);
    //получение шапки
    QStringList lst;
    for(int i=1; i<=cols; i++)
    {
        // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
        QAxObject* cell = sheet->querySubObject("Cells(QVariant,QVariant)", 1, i);
        // получение содержимого
        QVariant result = cell->property("Value");
        lst.append(result.toString());
        // освобождение памяти
        delete cell;
    }
    tbl->setHorizontalHeaderLabels(lst);

    //заполнение таблицы TableWidget
    for(int row=2; row<=rows; row++)
    {
        tbl->insertRow(tbl->rowCount());
        for(int col=1; col<=cols; col++)
        {
            // получение указателя на ячейку [row][col] ((!)нумерация с единицы)
            QAxObject* cell = sheet->querySubObject("Cells(QVariant,QVariant)", row, col);
            // получение содержимого
            QVariant result = cell->property("Value");
//            qDebug() << row << col << result.toString();
            ptwi = new QTableWidgetItem(result.toString());
            tbl->setItem(row-2, col-1, ptwi);
            // освобождение памяти
            delete cell;
        }
    }

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

    emit rowsReaded(rows);
    emit finished();
}

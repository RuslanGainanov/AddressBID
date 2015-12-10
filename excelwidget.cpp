#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _xlsParser(nullptr)
{
    _ui->setupUi(this);
    _dialog.setCancelButtonText(trUtf8("Отмена"));

    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     &_dialog, SLOT(reset()));
    QObject::connect(&_dialog, SIGNAL(canceled()),
                     &_futureWatcher, SLOT(cancel()));
    QObject::connect(&_futureWatcher, SIGNAL(progressRangeChanged(int,int)),
                     &_dialog, SLOT(setRange(int,int)));
    QObject::connect(&_futureWatcher, SIGNAL(progressValueChanged(int)),
                     &_dialog, SLOT(setValue(int)));
    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     this, SLOT(onProcessOfOpenFinished()));
}

ExcelWidget::~ExcelWidget()
{
    delete _ui;
    foreach (QString key, _data.keys()) {
        delete _data.take(key);
    }
}

void ExcelWidget::open()
{
    QString str =
            QFileDialog::getOpenFileName(this,
                                         trUtf8("Укажите исходный файл"),
                                         "",
                                         tr("Excel (*.xls *.xlsx)"));
    if(str.isEmpty())
        return;
    _ui->_lineEditFilename->setText(str);
    runThread(str);
}

QVariant ExcelWidget::openExcelFile(QString filename, int maxCountRows)
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "openExcelFile BEGIN"
             << QThread::currentThread()->currentThreadId()
             << currTime;

    HRESULT h_result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    switch(h_result)
    {
    case S_OK:
        qDebug("TestConnector: The COM library was initialized successfully on this thread");
        break;
    case S_FALSE:
        qWarning("TestConnector: The COM library is already initialized on this thread");
        break;
    case RPC_E_CHANGED_MODE:
        qWarning() << "TestConnector: A previous call to CoInitializeEx specified the concurrency model for this thread as multithread apartment (MTA)."
                   << " This could also indicate that a change from neutral-threaded apartment to single-threaded apartment has occurred";
        break;
    }

    // получаем указатель на Excel
    QScopedPointer<QAxObject> excel(new QAxObject("Excel.Application"));
    if(excel.isNull())
    {
        QString error="Cannot get Excel.Application";
        return QVariant(error);
    }

    QScopedPointer<QAxObject> workbooks(excel->querySubObject("Workbooks"));
    if(workbooks.isNull())
    {
        QString error="Cannot query Workbooks";
        return QVariant(error);
    }

    // на директорию, откуда грузить книгу
    QScopedPointer<QAxObject> workbook(workbooks->querySubObject(
                                           "Open(const QString&)",
                                           filename)
                                       );
    if(workbook.isNull())
    {
        QString error=
                QString("Cannot query workbook.Open(const %1)")
                .arg(filename);
        return QVariant(error);
    }

    QScopedPointer<QAxObject> sheets(workbook->querySubObject("Sheets"));
    if(sheets.isNull())
    {
        QString error="Cannot query Sheets";
        return QVariant(error);
    }

    int count = sheets->dynamicCall("Count()").toInt(); //получаем кол-во листов
    QStringList readedSheetNames;
    //читаем имена листов
    for (int i=1; i<=count; i++)
    {
        QScopedPointer<QAxObject> sheetItem(sheets->querySubObject("Item(int)", i));
        if(sheetItem.isNull())
        {
            QString error="Cannot query Item(int)"+QString::number(i);
            return QVariant(error);
        }
        readedSheetNames.append( sheetItem->dynamicCall("Name()").toString() );
        sheetItem->clear();
    }
    // проходим по всем листам документа
    int sheetNumber=0;
    ExcelDocument data;
    foreach (QString sheetName, readedSheetNames)
    {
        QScopedPointer<QAxObject> sheet(
                    sheets->querySubObject("Item(const QVariant&)",
                                           QVariant(sheetName))
                    );
        if(sheet.isNull())
        {
            QString error=
                    QString("Cannot query Item(const %1)")
                    .arg(sheetName);
            return QVariant(error);
        }

        QScopedPointer<QAxObject> usedRange(sheet->querySubObject("UsedRange"));
        QScopedPointer<QAxObject> usedRows(usedRange->querySubObject("Rows"));
        QScopedPointer<QAxObject> usedCols(usedRange->querySubObject("Columns"));
        int rows = usedRows->property("Count").toInt();
        int cols = usedCols->property("Count").toInt();

        //если на данном листе всего 1 строка (или меньше), то данный лист пуст => зачем он нам?
        if(rows>1)
        {
            //чтение данных
            ExcelSheet excelSheet; //таблица
            for(int row=1; row<=rows; row++)
            {
                QStringList rowList; //строка с данными
                for(int col=1; col<=cols; col++)
                {
                    QScopedPointer<QAxObject> cell (
                                sheet->querySubObject("Cells(QVariant,QVariant)",
                                                      row,
                                                      col)
                                );
                    QString result = cell->property("Value").toString();
                    rowList.append(result);
                    cell->clear();
                }
                excelSheet.append(rowList);

                //оставливаем обработку если получено нужное количество строк
                if(maxCountRows>0 && row-1 >= maxCountRows)
                    break;
            }
            data.insert(sheetName, excelSheet); //добавляем таблицу в документ
        }
        usedRange->clear();
        usedRows->clear();
        usedCols->clear();
        sheet->clear();
        sheetNumber++;
    }//end foreach _sheetNames

    sheets->clear();
    workbook->clear();
    workbooks->dynamicCall("Close()");
    workbooks->clear();
    excel->dynamicCall("Quit()");

    qDebug() << "openExcelFile END"
             << QThread::currentThread()->currentThreadId()
             << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    return QVariant::fromValue(data);
}

void ExcelWidget::onProcessOfOpenFinished()
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget onProcessOfOpenFinished" << currTime;

    if(_futureWatcher.isFinished()
            && !_futureWatcher.isCanceled())
    {
        QVariant result = _futureWatcher.future().result();
        if(result.isValid())
        {
            if(result.canConvert< ExcelDocument >())
            {
                ExcelDocument data = result.value<ExcelDocument>();
                foreach (QString sheetName, data.keys()) {
                    _data.insert(sheetName, new TableModel(sheetName));
                    QList<QStringList> rows = data[sheetName];
                    int nRow=0;
                    foreach (QStringList row, rows) {
                        if(nRow==0)
                        {
                            emit toDebug(objectName(),
                                         QString("Прочитана шапка у листа %1").arg(sheetName));
                            for(int col=0; col < row.size(); col++)
                                _data.value(sheetName)->setHeaderData(col, Qt::Horizontal, row.at(col));

                        }
                        else
                        {
                            TableModel *tm = _data.value(sheetName);
                            tm->insertRow(tm->rowCount());
                            for(int col=0; col<row.size(); col++)
                                tm->setData(tm->index(nRow-1, col),
                                            QVariant::fromValue(row.at(col)));
                            //emit rowReaded(sheetName, nRow);
                        }
                        nRow++;
                    }

                    TableView *view = new TableView(_ui->_tabWidget);
                    view->setModel(_data.value(sheetName));
                    _ui->_tabWidget->addTab(view, sheetName);
                }
            }
            else if(result.canConvert< QString >())
            {
                QString error = result.toString();
                emit toDebug(objectName(), error);
                emit errorOccured(objectName(), -1, error);
            }
        }
    }
}

void ExcelWidget::runThread(QString openFilename)
{
    qDebug() << "ExcelWidget runThread" << this->thread()->currentThreadId();

    QString name = QFileInfo(openFilename).fileName();
    _dialog.setLabelText(
                trUtf8("Открывается файл \"%1\". Ожидайте ...")
                .arg(name)
                        );
    QFuture<QVariant> f1 = QtConcurrent::run(this,
                                             &ExcelWidget::openExcelFile,
                                             openFilename,
                                             MAX_OPEN_IN_ROWS);
    // Start the computation.
    _futureWatcher.setFuture(f1);
    _dialog.exec();

    emit working();
}

void ExcelWidget::onHeadParsed(QString sheet, MapAddressElementPosition head)
{
    emit toDebug(objectName(),
                 "ExcelWidget onHeadParsed "
                 +sheet
                 );
    emit headParsed(sheet, head);
}

void ExcelWidget::onParseRow(QString sheet, int rowNumber, Address a)
{
    Q_UNUSED(a);
    emit toDebug(objectName(),
                 "ExcelWidget onParseRow "
                 +sheet+" "
                 +QString::number(rowNumber)
//                 +"\nstr:"+a.getStreet()+"\nb:"+a.getBuild()+"\nk:"+a.getKorp()
                 );
    emit rowParsed(sheet, rowNumber);
}

void ExcelWidget::onFinishParser()
{
//    emit toDebug(objectName(),
//                 "ExcelWidget::onFinishParser()"
//                 );
}

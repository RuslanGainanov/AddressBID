#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _xlsParser(nullptr),
    _isOneColumn(false)
{
    _ui->setupUi(this);
    _dialog.setCancelButtonText(trUtf8("Отмена"));
    QObject::connect(&_futureWatcher, SIGNAL(started()),
                     &_dialog, SLOT(show()));
    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     &_dialog, SLOT(hide()));
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
    _dialog.hide();
}

ExcelWidget::~ExcelWidget()
{
    delete _ui;
    foreach (QString key, _data.keys()) {
        delete _data.take(key);
    }
    if(_xlsParser)
        delete _xlsParser;
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
    runThreadOpen(str);
}

void ExcelWidget::parse()
{
    runThreadParsing();
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
    qDebug() << "ExcelWidget onProcessOfOpenFinished BEGIN"
             << currTime
             << this->thread()->currentThreadId();

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
                    _views.insert(sheetName, new TableView(_ui->_tabWidget));
                    _views[sheetName]->setModel(_data[sheetName]);
                    ExcelSheet rows = data[sheetName];
                    int nRow=0;
                    foreach (QStringList row, rows) {
                        if(nRow==0)
                            onHeadRead(sheetName, row);
                        else
                            onRowRead(sheetName, nRow-1, row);
                        nRow++;
                    }
                    onSheetRead(sheetName);
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
    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget onProcessOfOpenFinished END"
             << currTime
             << this->thread()->currentThreadId();
    emit finished();
}

void ExcelWidget::runThreadOpen(QString openFilename)
{
    qDebug() << "ExcelWidget runThreadOpen" << this->thread()->currentThreadId();

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

void ExcelWidget::runThreadParsing()
{
//    XlsParser *parser = new XlsParser;
//    QThread *thread = new QThread;
//    parser->setTableOfData(&_data);
//    parser->moveToThread(thread);
//    connect(thread, SIGNAL(started()),
//            parser, SLOT(process()));
//    connect(parser, SIGNAL(rowParsed(QString,int,Address)),
//            this, SLOT(onRowParsed(QString,int,Address)));
//    connect(parser, SIGNAL(headParsed(QString,MapAddressElementPosition)),
//            this, SLOT(onHeadParsed(QString,MapAddressElementPosition)));
//    connect(parser, SIGNAL(sheetParsed(QString)),
//            this, SLOT(onSheetParsed(QString)));
//    connect(parser, SIGNAL(appendColumn(int,QString)),
//            this, SLOT(onAppendColumn(int,QString)));
//    connect(parser, SIGNAL(notFoundMandatoryColumn(AddressElements,QString)),
//            this, SLOT(onNotFoundMandatoryColumn(AddressElements,QString)));
//    connect(parser, SIGNAL(finished()),
//            this, SLOT(onFinishParser()));
//    connect(parser, SIGNAL(finished()),
//            thread, SLOT(quit()));
//    connect(thread, SIGNAL(finished()),
//            parser, SLOT(deleteLater()));
//    connect(thread, SIGNAL(finished()),
//            thread, SLOT(deleteLater()));
//    thread->start();
}

void ExcelWidget::onRowRead(const QString &sheet, const int &nRow, QStringList &row)
{
    TableModel *tm = _data[sheet];
    tm->insertRow(tm->rowCount());
    for(int col=0; col<row.size(); col++)
        tm->setData(tm->index(nRow, col),
                    QVariant::fromValue(row.at(col)));
    //emit rowReaded(sheetName, nRow);
}

void ExcelWidget::onHeadRead(const QString &sheet, QStringList &head)
{
    emit toDebug(objectName(),
                 QString("Прочитана шапка у листа \"%1\"")
                 .arg(sheet));
    TableModel *tm = _data[sheet];
    for(int col=0; col < head.size(); col++)
        tm->setHeaderData(
                    col, Qt::Horizontal, head.at(col));

    QString colname = MapColumnNames[ STREET ];
    if(!head.contains(colname))//если остуствует столбец с данными о улице (городе и пр.)
    {
        emit onNotFoundMandatoryColumn(sheet, STREET, colname);
        return;
    }
    colname = MapColumnNames[ STREET_ID ];
    if(head.indexOf(colname)==-1)
    {
        int nCol = head.size();
//        tm->insertColumn(nCol);
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
        head.append(colname);
    }
    colname = MapColumnNames[ BUILD_ID ];
    if(!head.contains(colname))
    {
        int nCol = head.size();
//        tm->insertColumn(nCol);
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
        head.append(colname);
    }
    if(!head.contains(MapColumnNames[ BUILD ])
            && !head.contains(MapColumnNames[ KORP ]))
        _isOneColumn=false;
    else
        _isOneColumn=true;
    QMap<AddressElements, QString>::const_iterator it = MapColumnNames.begin();
//    while(it!=MapColumnNames.end())
//    {
//        int pos = head.indexOf(it.value());
//        if(pos!=-1)
//            _mapHead[sheet].insert(it.key(), pos);
//        it++;
//    }
    it = MapColumnParsedNames.begin();
    int nCol = head.size();
    while(it!=MapColumnParsedNames.end())
    {
        colname = it.value();
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
        onHideColumn(sheet, nCol); //скрываем столбец с распарсенным элементами
        head.append(colname);
//        _mapPHead[sheet].insert(it.key(), nCol);
        nCol++;
        it++;
    }
}

void ExcelWidget::onSheetRead(const QString &sheet)
{
    emit toDebug(objectName(),
                 QString("Лист \"%1\" прочитан")
                 .arg(sheet));
    TableView *view = _views[sheet];
    assert(view);
    int index = _ui->_tabWidget->addTab(view, sheet);
    _sheetIndex.insert(sheet, index);
}

void ExcelWidget::onHideColumn(const QString &sheet, int column)
{
    emit toDebug(objectName(),
                  "ExcelWidget onHideColumn "
                  +sheet+" column"
                  +QString::number(column)
                  );
    TableView *view = _views[sheet];
    assert(view);
    view->setColumnHidden(column, true);
}

void ExcelWidget::onHeadParsed(QString sheet, MapAddressElementPosition head)
{
    emit toDebug(objectName(),
                 "ExcelWidget onHeadParsed "
                 +sheet
                 );
    emit headParsed(sheet, head);
}

void ExcelWidget::onRowParsed(QString sheet, int rowNumber, Address a)
{
    Q_UNUSED(a);
    emit toDebug(objectName(),
                 "ExcelWidget::onRowParsed "
                 +sheet+" row:"
                 +QString::number(rowNumber)+"\n"/*+a.toString(PARSED)*/
                 );
    emit rowParsed(sheet, rowNumber);
}

void ExcelWidget::onSheetParsed(QString sheet)
{
    emit toDebug(objectName(),
                 "ExcelWidget::onFinishParser(QString)"+sheet
                 );
    emit sheetParsed(sheet);
}

void ExcelWidget::onFinishParser()
{
    emit toDebug(objectName(),
                 "ExcelWidget::onFinishParser()"
                 );
}

void ExcelWidget::onAppendColumn(int nCol, QString nameCol)
{
    emit toDebug(objectName(),
                 "ExcelWidget::onAppendColumn() "
                 +QString::number(nCol)+"\n"+nameCol
                 );
}

void ExcelWidget::onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName)
{
    emit toDebug(objectName(),
                 "ExcelWidget::onNotFoundMandatoryColumn()"
                 +sheet
                 +QString::number(ae)+"\n"+colName
                 );
}

#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _parser(nullptr),
    _thread(nullptr),
//    _db(nullptr),
    _delegateFounded(new SimpleDelegate(QBrush(QColor(FoundedColor)))),
    _delegateNotFounded(new SimpleDelegate(QBrush(QColor(NotFoundedColor))))
{
    _ui->setupUi(this);

    //excel.cpp
    _parser = new XlsParser;
    _thread = new QThread;
    _logFile.setFileName(LogFileName);
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    _logStream.setDevice(&_logFile);

//    _parser->moveToThread(_thread);
    connect(this, SIGNAL(rowReaded(QString,int,QStringList)),
            _parser, SLOT(onReadRow(QString,int,QStringList)));
    connect(this, SIGNAL(headReaded(QString,MapAddressElementPosition)),
            _parser, SLOT(onReadHead(QString,MapAddressElementPosition)));
    connect(_parser, SIGNAL(rowParsed(QString,int,Address)),
            this, SLOT(onRowParsed(QString,int,Address)));
    connect(_thread, SIGNAL(finished()),
            _parser, SLOT(deleteLater()));
    connect(_thread, SIGNAL(finished()),
            _thread, SLOT(deleteLater()));
    _thread->start();

    connect(&_futureWatcher, SIGNAL(finished()),
            this, SLOT(onProcessOfOpenFinished()));

//    connect(this, SIGNAL(toDebug(QString,QString)),
//            this, SLOT(onDebug(QString,QString)));

    connect(this, SIGNAL(toFile(QString,QString)), SLOT(onFile(QString,QString)));

    emit toDebug("test","test");

    //old
    _ui->_lineEditFilename->hide();

//    connect(this, SIGNAL(parserFinished()),
//            this, SLOT(onFinishParser()));

    connect(_ui->_parseWidget, SIGNAL(rowRemoved(QString,int)),
            this, SLOT(onRemoveRow(QString,int)));
    connect(_ui->_parseWidget, SIGNAL(dataChanged(QString,int,MapAEValue)),
            this, SLOT(onParsedDataChanged(QString,int,MapAEValue)));
    connect(this, SIGNAL(currentRowChanged(QString,int,MapAEValue)),
            _ui->_parseWidget, SLOT(onCurrentRowChanged(QString,int,MapAEValue)));

//    parse();
}

ExcelWidget::~ExcelWidget()
{
    delete _ui;
    foreach (QString key, _data.keys()) {
        delete _data.take(key);
    }
    if(_parser)
        delete _parser;
    delete _delegateFounded;
    delete _delegateNotFounded;
    _thread->quit();
    _thread->wait();
    closeLog();
}

void ExcelWidget::closeLog()
{
    if(_logFile.isOpen())
        _logFile.close();
}

void ExcelWidget::open()
{
    QString str =
            QFileDialog::getOpenFileName(this,
                                         trUtf8("Укажите исходный файл"),
                                         "",
                                         tr("Excel (*.xls *.xlsx *.csv)"));
    if(str.isEmpty())
        return;
    _ui->_lineEditFilename->setText(str);
    runThreadOpen(str);
}

QVariant ExcelWidget::openCsvFile(QString filename, int maxCountRows)
{
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "openCsvFile BEGIN"
//             << QThread::currentThread()->currentThreadId()
//             << currTime;
    QStringList data;
    QFile file1(filename);
    if (!file1.open(QIODevice::ReadOnly))
    {
        qDebug().noquote() << "Ошибка открытия для чтения";
        return data;
    }
    QTextStream in(&file1);
    QTextCodec *defaultTextCodec = QTextCodec::codecForName("Windows-1251");
    if (defaultTextCodec)
      in.setCodec(defaultTextCodec);
    int nRow=0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(!line.isEmpty())
        {
//            if(nRow==0)
//            {
//                qDebug().noquote() << "H:" << line;
//            }
            data.append(line);
            nRow++;
        }
        //оставливаем обработку если получено нужное количество строк
        if(maxCountRows>0 && nRow >= maxCountRows)
            break;
    }
    file1.close();

//    qDebug() << "openCsvFile END"
//             << QThread::currentThread()->currentThreadId()
//             << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    return QVariant::fromValue(data);
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

void ExcelWidget::parse()
{
    qDebug() << "!!!!" << "parse";
//    runThreadParsing();
}

void ExcelWidget::search()
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget search BEGIN"
             << currTime
             << this->thread()->currentThreadId();

//    qDebug() << "ExcelWidget search" << this->thread()->currentThreadId();
//    assert(_db);
    if(_data.isEmpty())
        return;
    if(_data2.isEmpty())
    {
        return;
    }

    QString sheetName =
            _ui->_tabWidget->tabText(
                _ui->_tabWidget->currentIndex()
                );
    _searchingSheetName=sheetName;
    if(_data2.contains(_searchingSheetName))
    {
        for(int i=0; i<_data2[sheetName].size(); i++)
        {
            if(!_data2[sheetName].at(i).isEmpty())
                emit findRowInBase(sheetName, i, _data2[sheetName].at(i));
//            if(i%200==0)
                emit toDebug(objectName(), QString("Обработано строк %1").arg(QString::number(i+1)));
        }
    }
    else
    {
        emit toDebug(objectName(),
                     "_data2 not contains "+_searchingSheetName);
    }

    //old !!!!
//    QProgressDialog *dialog = new QProgressDialog;
//    dialog->setWindowTitle(trUtf8("Ищем..."));
//    dialog->setLabelText(trUtf8("Производится поиск строк из вкладки \"%1\". Ожидайте ...")
//                         .arg(sheetName));
//    dialog->setCancelButtonText(trUtf8("Отмена"));

////    QObject::connect(&_futureWatcherS, SIGNAL(finished()),
////                     this, SLOT(onProcessOfSearchFinished()));
//    QObject::connect(dialog, SIGNAL(canceled()),
//                     &_futureWatcherS, SLOT(cancel()));
//    QObject::connect(&_futureWatcherS, SIGNAL(progressRangeChanged(int,int)),
//                     dialog, SLOT(setRange(int,int)));
//    QObject::connect(&_futureWatcherS, SIGNAL(progressValueChanged(int)),
//                     dialog, SLOT(setValue(int)));
//    QObject::connect(&_futureWatcherS, SIGNAL(finished()),
//                     dialog, SLOT(deleteLater()));

//    TableModel *tm = _data.value(sheetName, 0);
//    assert(tm);
//    ExcelSheet es = tm->getExcelSheet();
//    ListAddress listAddr;
//    MapAddressElementPosition addrPos = _mapPHead.value(sheetName);
//    MapAddressElementPosition addrPosOrig = _mapHead.value(sheetName);
//    foreach (QStringList row, es) {
//        Address a;
//        a.setTypeOfStreet(row.at(addrPos.value(TYPE_OF_STREET)));
//        a.setStreet(row.at(addrPos.value(STREET)));
//        a.setBuild(row.at(addrPos.value(BUILD)));
//        a.setKorp(row.at(addrPos.value(KORP)));
//        a.setLitera(row.at(addrPos.value(LITERA)));
//        a.setTypeOfCity1(row.at(addrPos.value(TYPE_OF_CITY1)));
//        a.setTypeOfCity2(row.at(addrPos.value(TYPE_OF_CITY2)));
//        a.setCity2(row.at(addrPos.value(CITY2)));
//        a.setCity1(row.at(addrPos.value(CITY1)));
//        a.setStreetId(row.at(addrPosOrig.value(STREET_ID)));
//        a.setBuildId(row.at(addrPosOrig.value(BUILD_ID)));
//        //TODO add other columns
//        listAddr.append(a);
//    }

//    QFuture<ListAddress> f1 = QtConcurrent::run(_db,
//                                             &Database::search,
//                                             sheetName,
//                                             listAddr);
//    // Start the computation.
//    _futureWatcherS.setFuture(f1);
//    dialog->exec();

    emit searching();
    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget search END"
             << currTime
             << this->thread()->currentThreadId();
}

void ExcelWidget::onProcessOfSearchFinished()
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget onProcessOfSearchFinished BEGIN"
             << currTime
             << this->thread()->currentThreadId();

    if(_futureWatcherS.isFinished()
            && !_futureWatcherS.isCanceled())
    {
        ListAddress data = _futureWatcherS.future().result();
        int row=0;
        foreach (Address a, data) {
//            qDebug() << "row getBid & Sid" << row << a.getBuildId() << a.getStreetId();
            if(a.getBuildId()!=0 && a.getStreetId()!=0)
            {
                _views[_searchingSheetName]->setItemDelegateForRow(row, _delegateFounded);
            }
            else
            {
                _views[_searchingSheetName]->setItemDelegateForRow(row, _delegateNotFounded);
            }
            int colSid = _mapHead[_searchingSheetName].value(STREET_ID);
            int colBid = _mapHead[_searchingSheetName].value(BUILD_ID);
            TableModel *tm = _data[_searchingSheetName];
            assert(tm);
            tm->setData(tm->index(row, colSid),
                        a.getStreetId());
            tm->setData(tm->index(row, colBid),
                        a.getBuildId());
            row++;
        }
    }

    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget onProcessOfSearchFinished END"
             << currTime
             << this->thread()->currentThreadId();
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
        emit toDebug(objectName(), "onProcessOfOpenFinished finish");
        if(result.isValid())
        {
            emit toDebug(objectName(), "onProcessOfOpenFinished valid");
            if(result.canConvert< ExcelDocument >())
            {
                emit toDebug(objectName(), "onProcessOfOpenFinished canconvert");
                ExcelDocument data = result.value<ExcelDocument>();
//                runThreadParsing();
//                disconnect(_parser, SIGNAL(rowParsed(QString,int,Address)),
//                        this, SLOT(onCurrentRowChanged()));
                foreach (QString sheetName, data.keys()) {
                    _data.insert(sheetName, new TableModel(sheetName));
                    _views.insert(sheetName, new TableView(_ui->_tabWidget));
                    _views[sheetName]->setModel(_data[sheetName]);
                    _selections.insert(sheetName, new ItemSelectionModel(_data[sheetName], this));
                    _views[sheetName]->setSelectionModel(_selections[sheetName]);
                    connect(_selections[sheetName], SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                            _data[sheetName], SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));
                    connect(_data[sheetName], SIGNAL(currentRowChanged(QString,int,QStringList)),
                            this, SLOT(onCurrentRowChanged(QString,int,QStringList)));
                    ExcelSheet rows = data[sheetName];
                    _countRow.insert(sheetName, rows.size());
                    _countParsedRow.insert(sheetName, 0);
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
            else if(result.canConvert< QStringList >())
            {
                emit toDebug("excel", "canconvert QStringList");
                QStringList data = result.value<QStringList>();
                QString sheetName = "csv";
                _data.insert(sheetName, new TableModel(sheetName));
                _views.insert(sheetName, new TableView(_ui->_tabWidget));
                _views[sheetName]->setModel(_data[sheetName]);
                _selections.insert(sheetName, new ItemSelectionModel(_data[sheetName], this));
                _views[sheetName]->setSelectionModel(_selections[sheetName]);
                connect(_selections[sheetName], SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                        _data[sheetName], SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));
                connect(_data[sheetName], SIGNAL(currentRowChanged(QString,int,QStringList)),
                        this, SLOT(onCurrentRowChanged(QString,int,QStringList)));
                _countRow.insert(sheetName, data.size());
                _countParsedRow.insert(sheetName, 0);
                int nRow=0;
                foreach (QString line, data) {
                    QStringList row = line.split(';');
                    if(nRow==0)
                        onHeadRead(sheetName, row);
                    else
                        onRowRead(sheetName, nRow-1, row);
                    nRow++;
                }
                onSheetRead(sheetName);
            }
        }
    }
    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug() << "ExcelWidget onProcessOfOpenFinished END"
             << currTime
             << this->thread()->currentThreadId();
    emit finished();
}

void ExcelWidget::onFounedAddress(QString sheetName, int nRow, Address addr)
{
    _data2[sheetName][nRow]=addr;
    emit toDebug("", QString::number(nRow)+";"+addr.toCsv());
    _views[sheetName]->setItemDelegateForRow(nRow, _delegateFounded);
    int colSid = _mapHead[sheetName].value(STREET_ID);
    int colBid = _mapHead[sheetName].value(BUILD_ID);
    TableModel *tm = _data[sheetName];
    assert(tm);
    tm->setData(tm->index(nRow, colSid),
                addr.getStreetId());
    tm->setData(tm->index(nRow, colBid),
                addr.getBuildId());
//    emit toFile("", QString::number(nRow+1)+";"+addr.toCsv()+"\n");
}

void ExcelWidget::onNotFounedAddress(QString sheetName, int nRow, Address addr)
{
//    _data2[sheetName][nRow]=addr;
    Q_UNUSED(sheetName);
    Q_UNUSED(addr);
    emit toDebug("", QString::number(nRow)+";!NOT_FOUND!");
//    emit toFile("", QString::number(nRow+1)+";!NOT_FOUND!"+"\n");
    _views[sheetName]->setItemDelegateForRow(nRow, _delegateNotFounded);
}

void ExcelWidget::onRemoveRow(QString sheet, int nRow)
{
    qDebug() << "ExcelWidget onRemoveRow" << sheet << nRow;
    TableModel *tm = _data.value(sheet, 0);
    assert(tm);
    if(nRow>=0 && nRow<tm->rowCount())
        qDebug() << tm->removeRow(nRow);
    else
        qDebug() << false;
}

void ExcelWidget::onCurrentRowChanged()
{
    QString sheet = _ui->_tabWidget->tabText(_ui->_tabWidget->currentIndex());
    if(_data.isEmpty())
        return;
    TableModel *tm = _data.value(sheet, 0);
    assert(tm);
    ItemSelectionModel *ism = _selections.value(sheet, 0);
    int nRow = ism->selectedIndexes().first().row();
    QStringList row = tm->getRow(nRow);

    onCurrentRowChanged(sheet, nRow, row);
}

void ExcelWidget::onCurrentRowChanged(QString sheet, int nRow,
                                      QStringList row)
{
//    qDebug() << "ExcelWidget onCurrentRowChanged"
//             << this->thread()->currentThreadId()
//             << nRow
//             << row;

    MapAEValue data;
    foreach (AddressElements ae, MapColumnParsedNames.keys()) {
        QString param = row.value(
                    _mapPHead.value(sheet).value(ae), QString()
                    );
        data.insert(ae, param);
    }

    emit currentRowChanged(sheet, nRow, data);
}

void ExcelWidget::onParsedDataChanged(QString sheet, int nRow,
                                      MapAEValue row)
{
    foreach (AddressElements ae, row.keys()) {
        TableModel *tm=_data[sheet];
        assert(tm);
        QString param = row.value(ae).toLower();
        tm->setData(tm->index(nRow,
                              _mapPHead.value(sheet).value(ae)),
                    param
                    );
    }
}

void ExcelWidget::runThreadOpen(QString openFilename)
{
    qDebug() << "ExcelWidget runThreadOpen" << this->thread()->currentThreadId();

    QString name = QFileInfo(openFilename).fileName();

    QProgressDialog *dialog = new QProgressDialog;
    dialog->setWindowTitle(trUtf8("Открываю файл..."));
    dialog->setLabelText(trUtf8("Открывается файл \"%1\". Ожидайте ...")
                         .arg(name));
    dialog->setCancelButtonText(trUtf8("Отмена"));
    QObject::connect(dialog, SIGNAL(canceled()),
                     &_futureWatcher, SLOT(cancel()));
    QObject::connect(&_futureWatcher, SIGNAL(progressRangeChanged(int,int)),
                     dialog, SLOT(setRange(int,int)));
    QObject::connect(&_futureWatcher, SIGNAL(progressValueChanged(int)),
                     dialog, SLOT(setValue(int)));
    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     dialog, SLOT(deleteLater()));

    QFuture<QVariant> f1;
    if(openFilename.contains(".csv"))
        f1 = QtConcurrent::run(this,
                               &ExcelWidget::openCsvFile,
                               openFilename,
                               MAX_OPEN_IN_ROWS);
    else
        f1 = QtConcurrent::run(this,
                               &ExcelWidget::openExcelFile,
                               openFilename,
                               MAX_OPEN_IN_ROWS);
    // Start the computation.
    _futureWatcher.setFuture(f1);
    dialog->exec();

    emit working();
}

void ExcelWidget::runThreadParsing()
{    
    if(_parser)
        delete _parser;
    if(_thread)
    {
        _thread->quit();
        _thread->wait();
        delete _thread;
    }
    _parser = new XlsParser;
    _thread = new QThread;
//    _parser->moveToThread(_thread);
    connect(this, SIGNAL(rowReaded(QString,int,QStringList)),
            _parser, SLOT(onReadRow(QString,int,QStringList)));
    connect(this, SIGNAL(headReaded(QString,MapAddressElementPosition)),
            _parser, SLOT(onReadHead(QString,MapAddressElementPosition)));
    connect(this, SIGNAL(isOneColumn(bool)),
            _parser, SLOT(onIsOneColumn(bool)));
    connect(_parser, SIGNAL(rowParsed(QString,int,Address)),
            this, SLOT(onRowParsed(QString,int,Address)));
//    connect(_parser, SIGNAL(headParsed(QString,MapAddressElementPosition)),
//            this, SLOT(onHeadParsed(QString,MapAddressElementPosition)));
    connect(_parser, SIGNAL(sheetParsed(QString)),
            this, SLOT(onSheetParsed(QString)));
    connect(_thread, SIGNAL(finished()),
            _parser, SLOT(deleteLater()));
    connect(_thread, SIGNAL(finished()),
            _thread, SLOT(deleteLater()));
    _thread->start();
}

void ExcelWidget::onRowRead(const QString &sheet, const int &nRow, QStringList &row)
{
//    emit toDebug(objectName(),
//                 QString("onRowRead \"%1\" \"%2\"")
//                 .arg(sheet)
//                 .arg(row.join(";")));
    TableModel *tm = _data[sheet];
    tm->insertRow(tm->rowCount());
    for(int col=0; col<row.size(); col++)
        tm->setData(tm->index(nRow, col),
                    QVariant::fromValue(row.at(col)));
    emit rowReaded(sheet, nRow, row);
}

void ExcelWidget::onFile(QString objName, QString mes)
{
    Q_UNUSED(objName);
    _logStream << mes;
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

    //если остуствует столбец с данными о улице (городе и пр.)
    QString colname = MapColumnNames[ STREET ];    
    if(!head.contains(colname))
    {
        emit onNotFoundMandatoryColumn(sheet, STREET, colname);
        return;
    }
    colname = MapColumnNames[ STREET_ID ];
    if(head.indexOf(colname)==-1)
    {
        int nCol = head.size();
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
        head.append(colname);
    }
    colname = MapColumnNames[ BUILD_ID ];
    if(!head.contains(colname))
    {
        int nCol = head.size();
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
        head.append(colname);
    }
    if(head.contains(MapColumnNames[ BUILD ])
            && head.contains(MapColumnNames[ KORP ]))
    {
        emit isOneColumn(false);
    }
    else /*if(!head.contains(MapColumnNames[ BUILD ])
            && !head.contains(MapColumnNames[ KORP ]))*/
    {
        emit isOneColumn(true);
    }

    QMap<AddressElements, QString>::const_iterator it
            = MapColumnParsedNames.begin();
    int nCol = head.size();
    while(it!=MapColumnParsedNames.end())
    {
        colname = it.value();
        tm->setHeaderData(nCol, Qt::Horizontal, colname);
#ifdef HIDE_PARSED_COLUMNS
        onHideColumn(sheet, nCol); //скрываем столбец с распарсенным элементами
#endif
        head.append(colname);
        nCol++;
        it++;
    }

    QList<AddressElements> keys = MapColumnParsedNames.keys();
    foreach (AddressElements ae, keys) {
        QString colname;
        int pos=0;
        colname = MapColumnNames.value(ae);
        pos = head.indexOf(colname);
        if(pos!=-1)
            _mapHead[sheet].insert(ae, pos);
        colname = MapColumnParsedNames.value(ae);
        pos = head.indexOf(colname);
        if(pos!=-1)
            _mapPHead[sheet].insert(ae, pos);
    }
    if(_mapHead.contains(sheet))
        emit headReaded(sheet, _mapHead[sheet]);
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
//    emit toDebug(objectName(),
//                  "ExcelWidget onHideColumn "
//                  +sheet+" column"
//                  +QString::number(column)
//                  );
    TableView *view = _views[sheet];
    assert(view);
    view->setColumnHidden(column, true);
}

//void ExcelWidget::onHeadParsed(QString sheet, MapAddressElementPosition head)
//{
//    emit toDebug(objectName(),
//                 "ExcelWidget onHeadParsed "
//                 +sheet
//                 );
//    emit headParsed(sheet, head);
//}

void ExcelWidget::onRowParsed(QString sheet, int nRow, Address a)
{
//    emit toDebug(objectName(),
//                 "ExcelWidget::onRowParsed "
//                 +sheet+" row:"
//                 +QString::number(nRow)+"\n"+a.toString(PARSED)
//                 );
    _data2[sheet].insert(nRow, a); //excel
    TableModel *tm=_data.value(sheet, 0);
    assert(tm);
    int nCol=0;
    nCol = _mapPHead.value(sheet).value(FSUBJ);//получаем номер столбца
    tm->setData(tm->index(nRow, nCol), a.getFsubj());//заносим в ячейку распарсенный элемент

    nCol = _mapPHead.value(sheet).value(DISTRICT);
    tm->setData(tm->index(nRow, nCol), a.getDistrict());

    nCol = _mapPHead.value(sheet).value(TYPE_OF_CITY1);
    tm->setData(tm->index(nRow, nCol), a.getTypeOfCity1());

    nCol = _mapPHead.value(sheet).value(CITY1);
    tm->setData(tm->index(nRow, nCol), a.getCity1());

    nCol = _mapPHead.value(sheet).value(ADDITIONAL);
    tm->setData(tm->index(nRow, nCol), a.getAdditional());

    nCol = _mapPHead.value(sheet).value(TYPE_OF_STREET);
    tm->setData(tm->index(nRow, nCol), a.getTypeOfStreet());

    nCol = _mapPHead.value(sheet).value(STREET);
    tm->setData(tm->index(nRow, nCol), a.getStreet());

    nCol = _mapPHead.value(sheet).value(TYPE_OF_CITY2);
    tm->setData(tm->index(nRow, nCol), a.getTypeOfCity2());

    nCol = _mapPHead.value(sheet).value(CITY2);
    tm->setData(tm->index(nRow, nCol), a.getCity2());

    nCol = _mapPHead.value(sheet).value(BUILD);
    tm->setData(tm->index(nRow, nCol), a.getBuild());

    nCol = _mapPHead.value(sheet).value(KORP);
    tm->setData(tm->index(nRow, nCol), a.getKorp());

    nCol = _mapPHead.value(sheet).value(LITERA);
    tm->setData(tm->index(nRow, nCol), a.getLitera());

    emit rowParsed(sheet, nRow);
//    if(_countParsedRow[sheet]>=_countRow[sheet])
//        emit parserFinished();
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
    _thread->quit();
}

void ExcelWidget::onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName)
{
    emit toDebug(objectName(),
                 "ExcelWidget::onNotFoundMandatoryColumn()"
                 +sheet
                 +QString::number(ae)+"\n"+colName
                 );
    int n = QMessageBox::critical(0,
                                  "Внимание",
                                  QString("Невозможно найти столбец с именем \"%1\" на листе \"%2\".\n"
                                          "Добавьте столбец с данным именем или переименуйте один из столбцов \n"
                                          "и повторите операцию.")
                                  .arg(colName)
                                  .arg(sheet),
                                  QMessageBox::Ok
                                  );
    if (n == QMessageBox::Ok) {
        // Do it!
    }
}

void ExcelWidget::on__lineEditNewAddr_returnPressed()
{
    onAddNewAddr(_ui->_lineEditNewAddr->text());
    _ui->_lineEditNewAddr->clear();
}

void ExcelWidget::on_pushButton_clicked()
{
    onAddNewAddr(_ui->_lineEditNewAddr->text());
    _ui->_lineEditNewAddr->clear();
}

void ExcelWidget::onAddNewAddr(QString addr)
{
//    qDebug() << "ExcelWidget onAddNewAddr" << this->thread()->currentThreadId();
    QString sheet = _ui->_tabWidget->tabText(_ui->_tabWidget->currentIndex());
    if(_data.isEmpty())
        return;
    if(!_mapHead.value(sheet).contains(STREET))
        return;
    TableModel *tm = _data.value(sheet, 0);
    assert(tm);
    int nCol = _mapHead.value(sheet).value(STREET);
    int nRow = tm->rowCount();
    tm->insertRow(nRow);
    tm->setData(tm->index(nRow, nCol),
                    QVariant::fromValue(addr));
    QStringList row;
    row = tm->getRow(nRow);
    emit isOneColumn(false);
    emit rowReaded(sheet, nRow, row);
    TableView *tv = _views.value(sheet, 0);
    tv->selectRow(nRow);
    tv->scrollTo(tm->index(nRow, nCol));
    connect(_parser, SIGNAL(rowParsed(QString,int,Address)),
            this, SLOT(onCurrentRowChanged()), Qt::UniqueConnection);
}

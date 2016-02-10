#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _parser(nullptr),
    _thread(nullptr),
    _delegateFounded(new SimpleDelegate(QBrush(QColor(FoundedColor)))),
    _delegateNotFounded(new SimpleDelegate(QBrush(QColor(NotFoundedColor)))),
    _delegateRepeatFounded(new SimpleDelegate(QBrush(QColor(RepeatFoundedColor))))
{
    _ui->setupUi(this);

    //excel.cpp
    _parser = new XlsParser;
    _thread = new QThread;
    _parser->moveToThread(_thread);
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

//    connect(this, SIGNAL(searchFinished(QString)), SLOT(onSearchFinished(QString)));

    _ui->_lineEditFilename->hide();
    _ui->_lineEditNewAddr->hide();
    _ui->pushButton->hide();

    _parseWidget = _ui->_parseWidget;
    connect(_parseWidget, SIGNAL(rowRemoved(QString,int)),
            this, SLOT(onRemoveRow(QString,int)));
    connect(_parseWidget, SIGNAL(dataChanged(QString,int,MapAEValue)),
            this, SLOT(onParsedDataChanged(QString,int,MapAEValue)));
    connect(this, SIGNAL(currentRowChanged(QString,int,MapAEValue)),
            _parseWidget, SLOT(onCurrentRowChanged(QString,int,MapAEValue)));

    setObjectName("ExcelWidget");
}

ExcelWidget::~ExcelWidget()
{
    qDebug() << "  ~ExcelWidget() <";
    emit objectDeleted();
    delete _ui;
    foreach (QString key, _models.keys()) {
        /*delete*/ _models.take(key)->deleteLater();
    }
    delete _delegateFounded;
    delete _delegateNotFounded;
    delete _delegateRepeatFounded;
    _thread->quit();
//    _thread->wait();
    _parser->deleteLater();
    _thread->deleteLater();
    qDebug() << "  ~ExcelWidget() >";
}

bool ExcelWidget::save()
{
    if(_models.isEmpty())
        return false;

    QString filter="Comma-Separated Values (*.csv);;Excel (*.xls *.xlsx)";
    QString str =
            QFileDialog::getSaveFileName(this,
                                         trUtf8("Укажите куда сохранить результаты"),
                                         "",
                                         filter);
    if(str.isEmpty())
    {
        emit messageReady("Ошибка сохранения");
        return false;
    }
    if(!runThreadSave(str, getCurrentTab()))
    {
        emit messageReady("Ошибка сохранения");
        return false;
    }
    emit messageReady("Успешно сохранено");
    return true;
}

bool ExcelWidget::runThreadSave(const QString &filename, const QString &sheetName)
{
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget runThreadSave BEGIN"
//             << currTime
//             << this->thread()->currentThreadId();

    QString name = QFileInfo(filename).fileName();

    if(_models.isEmpty())
        return false;
    if(!_mapHead.contains(sheetName))
        return false;
    if(_models.value(sheetName, 0)==0)
        return false;

    emit saving(sheetName, filename);
    emit toDebug(objectName(),
                 QString("Сохранение вкладки '%1' в файл '%2'")
                 .arg(sheetName).arg(filename));
    emit messageReady(QString("Сохранение вкладки '%1' в файл '%2'")
                      .arg(sheetName).arg(name));

    QProgressDialog *dialog = new QProgressDialog;
    QFutureWatcher<bool> *futureWatcher = new QFutureWatcher<bool>();
    dialog->setWindowTitle(trUtf8("Сохранение вкладки '%1'...").arg(sheetName));
    dialog->setLabelText(trUtf8("Сохраняется в файл \"%1\". Ожидайте ...")
                         .arg(name));
    dialog->setCancelButtonText(trUtf8("Отмена"));
    QObject::connect(dialog, SIGNAL(canceled()),
                     futureWatcher, SLOT(cancel()));
    QObject::connect(futureWatcher, SIGNAL(progressRangeChanged(int,int)),
                     dialog, SLOT(setRange(int,int)));
    QObject::connect(futureWatcher, SIGNAL(progressValueChanged(int)),
                     dialog, SLOT(setValue(int)));
    QObject::connect(futureWatcher, SIGNAL(finished()),
                     dialog, SLOT(deleteLater()));
    QObject::connect(futureWatcher, SIGNAL(finished()),
                     futureWatcher, SLOT(deleteLater()));

    QFuture<bool> f1;
    if(filename.contains(QRegExp("\\.csv$")))
        f1 = QtConcurrent::run(this,
                               &ExcelWidget::saveToCsv,
                               filename,
                               sheetName);
    else if(filename.contains(QRegExp("(\\.xls$)|(\\.xlsx$)")))
        f1 = QtConcurrent::run(this,
                               &ExcelWidget::saveToExcel,
                               filename,
                               sheetName);
    else
        return false;

    // Start the computation.
    futureWatcher->setFuture(f1);
    dialog->exec();

//    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget runThreadSave END"
//             << currTime
//             << this->thread()->currentThreadId();

//    emit messageReady(QString("Сохранение завершено"));
    emit saveFinished(sheetName, filename);
    return f1.result();
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

    QString name = "["+ QFileInfo(str).fileName().remove(QRegExp("\\.csv|\\.xlsx|\\.xls")) +"]";
//    qDebug() << name;
    QStringList sheets = _models.keys();
    bool isContain=false;
    foreach (QString sheet, sheets) {
        if(sheet.contains(name,  Qt::CaseInsensitive))
            isContain=true;
    }
    if(isContain)
        emit messageReady("Данный документ уже открыт в программе");
    else
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
        emit toDebug(objectName(),
                QString("Ошибка открытия файла '%1' для чтения").arg(filename));
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
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "openExcelFile BEGIN"
//             << QThread::currentThread()->currentThreadId()
//             << currTime;
    //запускаем процесс EXCEL в отдельном потоке
    HRESULT h_result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    switch(h_result)
    {
    case S_OK:
//        qDebug("TestConnector: The COM library was initialized successfully on this thread");
        break;
    case S_FALSE:
        emit toDebug(objectName(), "TestConnector: The COM library is already initialized on this thread");
//        qWarning("TestConnector: The COM library is already initialized on this thread");
        break;
    case RPC_E_CHANGED_MODE:
        emit toDebug(objectName(),
                     "TestConnector: A previous call to CoInitializeEx specified the concurrency model for this thread as multithread apartment (MTA)."
                     " This could also indicate that a change from neutral-threaded apartment to single-threaded apartment has occurred");
//        qWarning() << "TestConnector: A previous call to CoInitializeEx specified the concurrency model for this thread as multithread apartment (MTA)."
//                   << " This could also indicate that a change from neutral-threaded apartment to single-threaded apartment has occurred";
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
            sheetName+=" ["+QFileInfo(filename).baseName()+"]";
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

//    qDebug() << "openExcelFile END"
//             << QThread::currentThread()->currentThreadId()
//             << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    return QVariant::fromValue(data);
}

void ExcelWidget::search()
{
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget search BEGIN"
//             << currTime
//             << this->thread()->currentThreadId();
    QString sheetName = getCurrentTab();
    if(!sheetName.isEmpty() && _data2.contains(sheetName))
    {
        _countRepatingRow.remove(sheetName);
        _searchingRows.remove(sheetName);

        Searcher *searcher = new Searcher(sheetName);
        QThread *thread = new QThread;
        searcher->moveToThread(thread);
//        connect(this, &ExcelWidget::startSearchThread,
//                thread, &QThread::start);
        connect(this, &ExcelWidget::searchInBase2,
                searcher, &Searcher::selectAddress);
        connect(searcher, &Searcher::addressFounded,
                this, &ExcelWidget::onFounedAddress);
        connect(searcher, &Searcher::addressNotFounded,
                this, &ExcelWidget::onNotFounedAddress);
        connect(searcher, &Searcher::finished,
                thread, &QThread::quit, Qt::DirectConnection);
//        connect(searcher, &Searcher::finished, [=](){
//            qDebug() << "Searcher::finished";
//            onSearchFinished(sheetName);
//        });
        connect(this, &ExcelWidget::objectDeleted,
                searcher, &Searcher::cancel, Qt::DirectConnection);
        connect(this, &ExcelWidget::objectDeleted,
                searcher, &Searcher::deleteLater, Qt::DirectConnection);
        connect(this, &ExcelWidget::objectDeleted,
                thread, &QThread::deleteLater, Qt::DirectConnection);
        connect(this, &ExcelWidget::finishSearchThread,
                searcher, &Searcher::cancel, Qt::DirectConnection);
        disconnect(this, &ExcelWidget::waitSearchThread, 0, 0);
        connect(this, &ExcelWidget::waitSearchThread, [=](int msec){
            if(thread)
                if(thread->isRunning()){
                    qDebug() << "ExcelWidget::finishSearchThread wait <" << msec << QDateTime::currentDateTime().time();
                    thread->wait(msec);
                    qDebug() << "ExcelWidget::finishSearchThread wait >" << QDateTime::currentDateTime().time();
                }
        });
//        connect(this, &ExcelWidget::finishSearchThread, [=](){
//            qDebug() << "ExcelWidget::finishSearchThread <" << searcher << thread->isRunning() << thread->isFinished();
//            if(searcher!=nullptr)
//                searcher->cancel();
//        });
        connect(thread, &QThread::finished, [=](){
//            qDebug() << "QThread::finished";
            onSearchFinished(sheetName);
        });
//        connect(thread, &QThread::finished,
//                this, &ExcelWidget::onSearchFinished, Qt::DirectConnection);
        connect(thread, &QThread::finished,
                thread, &QThread::deleteLater);
        connect(thread, &QThread::finished,
                searcher, &QThread::deleteLater);
        thread->start(QThread::HighPriority);
        emit toDebug(objectName(),
                     QString("Поиск %1 строк").arg(_data2[sheetName].size()));
        searcher->setCountRows(_data2[sheetName].size());
//        emit startSearchThread(QThread::InheritPriority);
        for(int i=0; i<_data2[sheetName].size(); i++)
        {
            bool needFind=true;
            if(!_data2[sheetName].at(i).isEmpty()
                    && _data2[sheetName].at(i).getBuildId()==0
                   /* && _data2[sheetName].at(i).getStreetId()==0*/)
            {
                QAbstractItemDelegate *delegate=_views[sheetName]->itemDelegateForRow(i);
                if(delegate && delegate==_delegateNotFounded) //если данная строка уже искалась и не найден результат
                {
                    needFind=false;
//                    continue;
                }
                if(needFind)
                {
                    _searchingRows[sheetName].insert(i);
//                    emit toDebug(objectName(), QString("Поиск строки %1").arg(QString::number(i)));
//                    emit searchInBase(sheetName, i, _data2[sheetName].at(i));
                }
                emit searchInBase2(sheetName, i, _data2[sheetName].at(i), needFind);
            }
            else
                emit searchInBase2(sheetName, i, _data2[sheetName].at(i), false);
        }

//        emit finishSearchThread();
        if(!_searchingRows.value(sheetName).isEmpty())
        {
            emit searching(sheetName);
        }
        else
            emit finishSearchThread();
    }
    else
    {
        emit toDebug(objectName(),
                     QString("Данные на вкладке '%1' отсутсвуют").arg(sheetName));
    }
//    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget search END"
//             << currTime
//             << this->thread()->currentThreadId();
}

void ExcelWidget::search2()
{
    QString sheetName = getCurrentTab();
    if(!sheetName.isEmpty() && _data2.contains(sheetName))
    {
        _countRepatingRow.remove(sheetName);
        _searchingRows.remove(sheetName);
        emit toDebug(objectName(),
                     QString("Поиск %1 строк").arg(_data2[sheetName].size()));
        for(int i=0; i<_data2[sheetName].size(); i++)
        {
            bool needFind=true;
            if(!_data2[sheetName].at(i).isEmpty()
                    && _data2[sheetName].at(i).getBuildId()==0)
            {
                QAbstractItemDelegate *delegate=_views[sheetName]->itemDelegateForRow(i);
                if(delegate && delegate==_delegateNotFounded) //если данная строка уже искалась и не найден результат
                    needFind=false;

                if(needFind)
                    _searchingRows[sheetName].insert(i);
            }
            else
                needFind=false;

            SearchStruct s;
            s.row=i;
            s.a=_data2[sheetName].at(i);
            s.sheet=sheetName;
            s.findIt=needFind;
            FutureWatcher<SearchStruct> *futureWatcher = new FutureWatcher<SearchStruct>;
            _listFuture.push_back(futureWatcher);
//            QFuture<SearchStruct> f1;
//            futureWatcher->setFuture(f1);
//            qDebug() << "_threadPool"
//                     << _threadPool.maxThreadCount()
//                     << _threadPool.activeThreadCount()
//                     << QThread::idealThreadCount();
//            connect(futureWatcher, &FutureWatcher<SearchStruct>::started, [=](){
//                qDebug() << "started" << futureWatcher << futureWatcher->isStarted();
//                qDebug() << "_threadPool"
//                         << _threadPool.maxThreadCount()
//                         << _threadPool.activeThreadCount()
//                         << QThread::idealThreadCount();
//            });
//            connect(futureWatcher, &FutureWatcher<SearchStruct>::finished, [=](){
//                qDebug() << "finished"
//                         << futureWatcher
//                         << futureWatcher->isFinished()
//                         << futureWatcher->isCanceled();
//                _listFuture.removeAll(futureWatcher);
//                futureWatcher->deleteLater();
//            });
//            connect(futureWatcher, &FutureWatcher<SearchStruct>::canceled, [=](){
//                qDebug() << "canceled"
//                         << futureWatcher
//                         << futureWatcher->isFinished()
//                         << futureWatcher->isCanceled();
//            });
            connect(futureWatcher, &FutureWatcher<SearchStruct>::finished,
                    this, &ExcelWidget::onFinishSearch);
//            connect(futureWatcher, &FutureWatcher<SearchStruct>::finished,
//                    futureWatcher, &FutureWatcher<SearchStruct>::deleteLater);
//            f1 = QtConcurrent::run(Searcher::concSelectAddress, s);
            //TODO: uncomment next row for use this function!
//            futureWatcher->setFuture(QtConcurrent::run(&_threadPool, Searcher::concSelectAddress, s));
        }

        if(!_searchingRows.value(sheetName).isEmpty())
            emit searching(sheetName);
        else
            emit finishSearchThread();
    }
    else
        emit toDebug(objectName(),
                     QString("Данные на вкладке '%1' отсутсвуют").arg(sheetName));
}


void ExcelWidget::stopSearch()
{
    emit toDebug(objectName(),
                 QString("Остановить поиск у '%1'").arg(getCurrentTab()));
    emit finishSearchThread();
}

void ExcelWidget::waitSearch()
{
//    QThread::currentThread()->msleep(500);
    emit waitSearchThread(2000);

    //search2:
//    foreach (FutureWatcher<SearchStruct> *fw, _listFuture) {
//        if(!fw->isFinished()){
//            qDebug() << "wait <" << fw << fw->isRunning() << fw->isStarted() << getCurrentTime();
//            fw->waitForFinished();
//            qDebug() << "wait >" << fw << fw->isRunning() << fw->isStarted() << getCurrentTime();
//            break;
//        }
//    }

//    //thread-pool:
//    qDebug() << "wait <" << getCurrentTime();
//    qDebug() <<  _threadPool.waitForDone();
//    qDebug() << "wait >" << getCurrentTime();
}

void ExcelWidget::onProcessOfOpenFinished()
{
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget onProcessOfOpenFinished BEGIN"
//             << currTime
//             << this->thread()->currentThreadId();

    bool canOpen=true;
    if(_futureWatcher.isFinished()
            && !_futureWatcher.isCanceled())
    {
        QVariant result = _futureWatcher.future().result();
//        emit toDebug(objectName(), "onProcessOfOpenFinished finish");
        if(result.isValid())
        {
//            emit toDebug(objectName(), "onProcessOfOpenFinished valid");
            if(result.canConvert< ExcelDocument >())
            {
//                emit toDebug(objectName(), "onProcessOfOpenFinished canconvert");
                ExcelDocument data = result.value<ExcelDocument>();
//                runThreadParsing();
//                disconnect(_parser, SIGNAL(rowParsed(QString,int,Address)),
//                        this, SLOT(onCurrentRowChanged()));
                foreach (QString sheetName, data.keys()) {
                    _models.insert(sheetName, new TableModel(sheetName));
                    _views.insert(sheetName, new QTableView(_ui->_tabWidget));
                    _views[sheetName]->setModel(_models[sheetName]);
                    _selections.insert(sheetName, new QItemSelectionModel(_models[sheetName], this));
                    _views[sheetName]->setSelectionModel(_selections[sheetName]);
                    connect(_selections[sheetName], SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                            _models[sheetName], SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));
                    connect(_models[sheetName], SIGNAL(currentRowChanged(QString,int,QStringList)),
                            this, SLOT(onCurrentRowChanged(QString,int,QStringList)));
                    connect(_models[sheetName], SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                            this, SLOT(onTableDataChanged(QModelIndex,QModelIndex,QVector<int>)));
                    ExcelSheet rows = data[sheetName];
                    _countRow.insert(sheetName, 0);
                    _countParsedRow.insert(sheetName, 0);
                    _countRepatingRow.insert(sheetName, 0);
                    int nRow=0;
                    foreach (QStringList row, rows) {
                        if(nRow==0)
                        {
                            if(!onHeadRead(sheetName, row))
                            {
                                canOpen=false;
                                break;
                            }
                        }
                        else
                            onRowRead(sheetName, nRow-1, row);
                        nRow++;
                    }
                    if(canOpen)
                        onSheetRead(sheetName);
//                    else
//                        emit messageReady(QString("Некорректный заголовок у таблицы '%1'")
//                                          .arg(sheetName));
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
//                emit toDebug("excel", "canconvert QStringList");
                QStringList data = result.value<QStringList>();
                QString sheetName = "Лист1 ["+QFileInfo(_openFilename).fileName().remove(".csv")+"]";
                _models.insert(sheetName, new TableModel(sheetName));
                _views.insert(sheetName, new QTableView(_ui->_tabWidget));
                _views[sheetName]->setModel(_models[sheetName]);
                _selections.insert(sheetName, new QItemSelectionModel(_models[sheetName], this));
                _views[sheetName]->setSelectionModel(_selections[sheetName]);
                connect(_selections[sheetName], SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                        _models[sheetName], SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));
                connect(_models[sheetName], SIGNAL(currentRowChanged(QString,int,QStringList)),
                        this, SLOT(onCurrentRowChanged(QString,int,QStringList)));
                connect(_models[sheetName], SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                        this, SLOT(onTableDataChanged(QModelIndex,QModelIndex,QVector<int>)));
                _countRow.insert(sheetName, 0);
                _countParsedRow.insert(sheetName, 0);
                _countRepatingRow.insert(sheetName, 0);
                int nRow=0;
                foreach (QString line, data) {
                    QStringList row = line.split(';');
                    if(nRow==0)
                    {
                        if(!onHeadRead(sheetName, row))
                        {
                            canOpen=false;
                            break;
                        }
                    }
                    else
                        onRowRead(sheetName, nRow-1, row);
                    nRow++;
                }
                if(canOpen)
                    onSheetRead(sheetName);
//                else
//                    emit messageReady(QString("Некорректный заголовок у таблицы '%1'")
//                                      .arg(sheetName));
            }
            else
                canOpen=false;
        }
        else
            canOpen=false;
    }
    else
        canOpen=false;
//    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug() << "ExcelWidget onProcessOfOpenFinished END"
//             << currTime
//             << this->thread()->currentThreadId();
    if(canOpen){
        emit openFinished(_openFilename);
    }
    else{
        emit openError(_openFilename);
    }
//    emit messageReady("Открытие завершено");
}

void ExcelWidget::onFinishSearch()
{
    FutureWatcher <SearchStruct> *fw = dynamic_cast<FutureWatcher<SearchStruct> *>(sender());
//    qDebug() << "finish" << fw << fw->isFinished() << fw->isCanceled();
    if(fw->isFinished()
            && !fw->isCanceled())
    {
        SearchStruct ss=fw->future().result();
//        qDebug() << ss.a.getBuildId()
//                 << ss.a.getStreet()
//                 << ss.row
//                 << ss.findIt
//                 << ss.found
//                 << ss.sheet;

        if(ss.findIt)
        {
            if(ss.found)
                onFounedAddress(ss.sheet, ss.row, ss.a);
            else
                onNotFounedAddress(ss.sheet, ss.row, ss.a);
        }
    }

    _listFuture.removeAll(fw);
    fw->deleteLater();
}

void ExcelWidget::onFounedAddress(QString sheetName, int nRow, Address addr)
{
    if(!_models.contains(sheetName)) //если данная вкладка не существует (удалена)
    {
        return;
    }
#ifdef SHOW_FOUNDED_ITEMS_TO_DEBUG
    emit toDebug(objectName(),
                 QString("Найдена строка %1: %2").arg(nRow).arg(addr.getStreet()));
#endif
    if(_searchingRows.contains(sheetName))
    {
        _searchingRows[sheetName].remove(nRow);
        if(_searchingRows.value(sheetName).isEmpty())
            emit searchFinished(sheetName);
    }
//    qDebug() << "onFounedAddress:"
//             << nRow
//             << addr.getStreet()
//             << _countRepatingRow[sheetName]
//             << _data2[sheetName].value(nRow).getBuildId();

    nRow+=_countRepatingRow[sheetName];
    TableModel *tm = _models[sheetName];
    QTableView *view = _views[sheetName];
    assert(tm);
    assert(view);

    if(_data2[sheetName].value(nRow).getBuildId()==0)
    {
        //значит данный адрес ранее не был найден (повторы отсутсвуют)
        _data2[sheetName][nRow]=addr;
        _views[sheetName]->setItemDelegateForRow(nRow, _delegateFounded);
    }
    else
    {
        _countRepatingRow[sheetName]++;
        _countParsedRow[sheetName]++;
        nRow++;
        _data2[sheetName].insert(nRow,addr);
//        _insertedRowAfterSearch[sheetName]=nRow;
        tm->insertRow(nRow);
        tm->setRow(nRow, tm->getRow(nRow-1));
        view->setItemDelegateForRow(nRow-1, _delegateRepeatFounded); //меняем цвет у предыдушей строки для которой найден повтор
        for(int i=tm->rowCount()-1; i>nRow; i--)
        {
            QAbstractItemDelegate *pastDelegate=view->itemDelegateForRow(i-1);
            if(pastDelegate)
                view->setItemDelegateForRow(i, pastDelegate);
            else
                view->setItemDelegateForRow(i, view->itemDelegate());
        }
        _views[sheetName]->setItemDelegateForRow(nRow, _delegateRepeatFounded);
    }
    tm->setData(tm->index(nRow, _mapHead[sheetName].value(STREET_ID)),
                addr.getStreetId());
    tm->setData(tm->index(nRow, _mapHead[sheetName].value(BUILD_ID)),
                addr.getBuildId());
    tm->setData(tm->index(nRow, _mapPHead[sheetName].value(RAW_ADDR)),
                addr.getRawAddressStringWithoutID());
}

void ExcelWidget::onNotFounedAddress(QString sheetName, int nRow, Address addr)
{
    Q_UNUSED(sheetName);
    Q_UNUSED(addr);
#ifdef SHOW_FOUNDED_ITEMS_TO_DEBUG
    emit toDebug(objectName(),
                 QString("Cтрока %1 не найдена!").arg(nRow));
#endif
    if(_searchingRows.contains(sheetName))
    {
        _searchingRows[sheetName].remove(nRow);
        if(_searchingRows.value(sheetName).isEmpty())
            emit searchFinished(sheetName);
    }
    nRow+=_countRepatingRow[sheetName];
    TableModel *tm = _models[sheetName];
    QTableView *view = _views[sheetName];
    assert(tm);
    assert(view);
    view->setItemDelegateForRow(nRow, _delegateNotFounded);
}

void ExcelWidget::onRemoveRow(QString sheet, int nRow)
{
    qDebug() << "ExcelWidget onRemoveRow" << sheet << nRow;
    TableModel *tm = _models.value(sheet, 0);
    assert(tm);
    if(nRow>=0 && nRow<tm->rowCount())
    {
        qDebug() << (tm->removeRow(nRow)? "success":"");
        _data2[sheet].removeAt(nRow);
        for(int i=nRow; i<tm->rowCount()-1; i++)
        {
            _views[sheet]->setItemDelegateForRow(
                        i,
                        _views[sheet]->itemDelegateForRow(i+1)
                        );
        }
    }
    else
        qDebug() << false;
}

void ExcelWidget::onTableDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
    QModelIndex &indx(topLeft);
    if(topLeft.isValid())
    {
//        emit toDebug(objectName(),
//                     indx.data().toString()+":"
//                     +QString::number(indx.row())+"|"+QString::number(indx.column())
//                     );
        const TableModel *model = qobject_cast<const TableModel *>(indx.model());
        assert(model);
        QString sheet(model->getName());
//        if(_insertedRowAfterSearch.contains(sheet)
//                && _insertedRowAfterSearch[sheet]==indx.row())
//        {
//            return;
//        }
        MapAddressElementPosition &map(_mapHead[sheet]);
        if(indx.row()>=_countParsedRow[sheet])
            return;
        bool editAddressElement=false;
        foreach (AddressElements ae, ListAddressElements) {
            if(ae==STREET_ID || ae==BUILD_ID /*|| ae==RAW_ADDR*/)
                continue;
            if(indx.column()==map.value(ae, -1))
            {
                editAddressElement=true;
                break;
            }
        }
        if(editAddressElement)
        {
            emit toDebug(objectName(),
                         QString("Ячейка [%1, %2] отредактирована:").arg(indx.row()).arg(indx.column())
                         +indx.data().toString());
            _editedRow[sheet]=indx.row();

            _views[sheet]->setItemDelegateForRow(indx.row(), _views[sheet]->itemDelegate());

            TableModel *m = const_cast<TableModel *>(model);
            _data2[sheet][indx.row()].setBuildId(0);
            _data2[sheet][indx.row()].setStreetId(0);
            int col=map.value(STREET_ID);
            m->setData(m->index(indx.row(), col),
                        QVariant::fromValue(QString()));
            col=map.value(BUILD_ID);
            m->setData(m->index(indx.row(), col),
                        QVariant::fromValue(QString()));
            col=_mapPHead[sheet].value(RAW_ADDR);
            m->setData(m->index(indx.row(), col),
                        QVariant::fromValue(QString()));

            emit rowReaded(sheet, indx.row(), model->getRow(indx.row()));
        }
    }
}


void ExcelWidget::onCurrentRowChanged()
{
//    qDebug() << "ExcelWidget onCurrentRowChanged";
    QString sheet = _ui->_tabWidget->tabText(_ui->_tabWidget->currentIndex());
    if(_models.isEmpty())
        return;
    TableModel *tm = _models.value(sheet, 0);
    assert(tm);
    QItemSelectionModel *ism = _selections.value(sheet, 0);
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
    _views[sheet]->setItemDelegateForRow(nRow, _views[sheet]->itemDelegate());
    int colSid = _mapHead[sheet].value(STREET_ID);
    int colBid = _mapHead[sheet].value(BUILD_ID);
    TableModel *tm = _models[sheet];
    assert(tm);
    tm->setData(tm->index(nRow, colSid),
                "");
    tm->setData(tm->index(nRow, colBid),
                "");
    _data2[sheet][nRow].setBuildId(0);
    _data2[sheet][nRow].setStreetId(0);

    foreach (AddressElements ae, row.keys()) {
        TableModel *tm=_models[sheet];
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
    emit toDebug(objectName(),
                 QString("Открывается файл '%1'").arg(openFilename));

    _openFilename=openFilename;
    QString name = QFileInfo(openFilename).fileName();
//    emit messageReady(QString("Открывается файл '%1'").arg(name));

    emit opening(openFilename);

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
}

void ExcelWidget::onRowRead(const QString &sheet, const int &nRow, QStringList &row)
{
//    emit toDebug(objectName(),
//                 QString("onRowRead \"%1\" \"%2\"")
//                 .arg(sheet)
//                 .arg(row.join(";")));
    TableModel *tm = _models[sheet];
    tm->insertRow(tm->rowCount());
    for(int col=0; col<row.size(); col++)
        tm->setData(tm->index(nRow, col),
                    QVariant::fromValue(row.at(col)));
    _countRow[sheet]++;
    emit rowReaded(sheet, nRow, row);
}

bool ExcelWidget::onHeadRead(const QString &sheet, QStringList &head)
{
//    emit toDebug(objectName(),
//                 QString("Прочитана шапка у листа '%1'")
//                 .arg(sheet));
    TableModel *tm = _models[sheet];
    for(int col=0; col < head.size(); col++)
        tm->setHeaderData(
                    col, Qt::Horizontal, head.at(col));

    //если остуствует столбец с данными о улице (городе и пр.)
    QString colname = MapColumnNames[ STREET ];    
    if(!head.contains(colname))
    {
        onNotFoundMandatoryColumn(sheet, STREET, colname);
        return false;
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
//    if(head.contains(MapColumnNames[ BUILD ])
//            && head.contains(MapColumnNames[ KORP ]))
//    {
//        emit isOneColumn(false);
//    }
//    else /*if(!head.contains(MapColumnNames[ BUILD ])
//            && !head.contains(MapColumnNames[ KORP ]))*/
//    {
//        emit isOneColumn(true);
//    }

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

    return true;
}

void ExcelWidget::onSheetRead(const QString &sheet)
{
    emit toDebug(objectName(),
                 QString("Лист '%1' прочитан")
                 .arg(sheet));
    QTableView *view = _views[sheet];
    assert(view);
    /*int index = */_ui->_tabWidget->addTab(view, sheet);
//    _sheetIndex.insert(sheet, index);
}

void ExcelWidget::onHideColumn(const QString &sheet, int column)
{
//    emit toDebug(objectName(),
//                  "ExcelWidget onHideColumn "
//                  +sheet+" column"
//                  +QString::number(column)
//                  );
    QTableView *view = _views[sheet];
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
//                 +QString::number(nRow)/*+"\n"+a.toDebug()*/
//                 );
    TableModel *tm=_models.value(sheet, 0);
    assert(tm);
    int nCol=0;
    nCol = _mapPHead.value(sheet).value(FSUBJ);//получаем номер столбца
    tm->setData(tm->index(nRow, nCol), a.getFsubj());//заносим в ячейку распарсенный элемент

    nCol = _mapPHead.value(sheet).value(TYPE_OF_FSUBJ);
    tm->setData(tm->index(nRow, nCol), a.getTypeOfFSubjInString());

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

    if(_editedRow.contains(sheet) && _editedRow[sheet]==nRow)
    {
        _editedRow.remove(sheet);
        _data2[sheet][nRow]=a;
        onCurrentRowChanged();
    }
    else
    {
//        if(_insertedRowAfterSearch.contains(sheet)
//                && _insertedRowAfterSearch[sheet]==nRow)
//        {
//            _insertedRowAfterSearch.remove(sheet);
//        }
//        else
//        {
            _data2[sheet].insert(nRow, a);
            _countParsedRow[sheet]++;
            emit rowParsed(sheet, nRow);
            if(_countParsedRow[sheet]>=_countRow[sheet])
                onSheetParsed(sheet);
//        }
    }

}

void ExcelWidget::onSheetParsed(QString sheet)
{
//    emit toDebug(objectName(),
//                 "ExcelWidget::onSheetParsed(QString)"+sheet
//                 );
    emit sheetParsed(sheet);
}

void ExcelWidget::onSearchFinished(QString sheet)
{
//    emit toDebug(objectName(),
//                 "ExcelWidget::onSearchFinished(): "+sheet
//                 );
    emit searchFinished(sheet);

}

void ExcelWidget::onNotFoundMandatoryColumn(QString sheet, AddressElements ae, QString colName)
{
    Q_UNUSED(ae);
    emit toDebug(objectName(),
                 QString("Невозможно найти столбец с именем '%1' на листе '%2'")
                 .arg(colName)
                 .arg(sheet)
                 );
    int n = QMessageBox::critical(0,
                                  "Внимание",
                                  trUtf8("Невозможно найти столбец с именем \"%1\" на листе \"%2\".\n"
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
    if(_models.isEmpty())
        return;
    if(!_mapHead.value(sheet).contains(STREET))
        return;
    TableModel *tm = _models.value(sheet, 0);
    assert(tm);
    int nCol = _mapHead.value(sheet).value(STREET);
    int nRow = tm->rowCount();
    tm->insertRow(nRow);
    tm->setData(tm->index(nRow, nCol),
                    QVariant::fromValue(addr));
    QStringList row;
    row = tm->getRow(nRow);
//    emit isOneColumn(false);
    emit rowReaded(sheet, nRow, row);
    QTableView *tv = _views.value(sheet, 0);
    tv->selectRow(nRow);
    tv->scrollTo(tm->index(nRow, nCol));
    connect(_parser, SIGNAL(rowParsed(QString,int,Address)),
            this, SLOT(onCurrentRowChanged()), Qt::UniqueConnection); //TODO add disconnect in onRowParsed
}

bool ExcelWidget::saveToCsv(const QString &filename, const QString &sheet)
{
    TableModel *tm = _models.value(sheet);
    assert(tm);
    QFile file1(filename);
    if (!file1.open(QIODevice::WriteOnly))
    {
        emit toDebug(objectName(),
                     "Ошибка открытия файла для записи");
        return false;
    }
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QStringList head(tm->getHeader());
    QString headLine;
    for(int i=0; i<head.size(); i++)
    {
        if(!head.at(i).contains("!P_"))
            headLine+=head.at(i)+";";
    }
    headLine.remove(headLine.size()-1,1);
    headLine.append("\r\n");
    QByteArray ba = codec->fromUnicode(headLine.toUtf8());
    file1.write(ba);
    for(int row=0; row<tm->rowCount(); row++)
    {
        QString line;
        for(int col=0; col<tm->columnCount(); col++)
        {
            QString colName(head.value(col));
            if(!colName.contains("!P_"))
            {
                line+=tm->data(tm->index(row, col)).toString().replace(';',',')+";";
            }
        }
        line.remove(line.size()-1,1);
        line.append("\r\n");
        QByteArray ba = codec->fromUnicode(line.toUtf8());
        file1.write(ba);
    }
    file1.close();
    return true;
}

bool ExcelWidget::saveToExcel(const QString &filename, const QString &sheetName)
{
    TableModel *tm = _models.value(sheetName);
    assert(tm);

    // получаем указатель на Excel
    QScopedPointer<QAxObject> excel(new QAxObject("Excel.Application"));
    if(excel.isNull())
    {
        emit toDebug(objectName(),
                     "Cannot get Excel.Application");
        return false;
    }
    QScopedPointer<QAxObject> workbooks(excel->querySubObject("Workbooks"));
    if(workbooks.isNull())
    {
        emit toDebug(objectName(), "Cannot query Workbooks");
        return false;
    }
    QScopedPointer<QAxObject> workbook(workbooks->querySubObject("Add()"));
    if(workbook.isNull())
    {
        emit toDebug(objectName(), "Cannot query workbook.Add()");
        return false;
    }
    QScopedPointer<QAxObject> sheets(workbook->querySubObject("Sheets"));
    if(sheets.isNull())
    {
        emit toDebug(objectName(), "Cannot query Sheets");
        return false;
    }

    int count = sheets->dynamicCall("Count()").toInt();
    QString firstSheetName;
    for (int i=1; i<=count; i++)
    {
        QScopedPointer<QAxObject> firstSheet(sheets->querySubObject("Item(int)", i));
        if(firstSheet.isNull())
        {
            emit toDebug(objectName(), "Cannot query Item(int)");
            return false;
        }
        firstSheetName = firstSheet->dynamicCall("Name()").toString();
        firstSheet->clear();
        break;
    }

    QScopedPointer<QAxObject> sheet;
    if(!firstSheetName.isEmpty())
        sheet.reset(sheets->querySubObject(
                    "Item(const QVariant&)", QVariant(firstSheetName)));
    if(sheet.isNull())
    {
        emit toDebug(objectName(), "Cannot query Item(const QVariant&)");
        return false;
    }

    // заполняем массив данными из текущей таблицы
    QList<QVariant> data;
    QStringList head(tm->getHeader());
    int cols=0;
    QList<QVariant> headLine;
    for(int i=0; i<head.size(); i++)
    {
        if(!head.at(i).contains("!P_"))
        {
           headLine.append(head.at(i));
           cols++;
        }
    }
    data.append(QVariant::fromValue(headLine));
    for(int row=0; row<tm->rowCount(); row++)
    {
        QList<QVariant> line;
        for(int col=0; col<tm->columnCount(); col++)
            if(!head.value(col).contains("!P_"))
                line.append(tm->data(tm->index(row, col)));
        data.append(QVariant::fromValue(line));
    }
    //***********

    //показать на экране
//    for(int row=0; row<data.size(); row++)
//    {
//        QList<QVariant> line = data.at(row).toList();
//        QString str;
//        for(int col=0; col<line.size(); col++)
//            str+=line.at(col).toString()+";";
//        qDebug() << str;
//    }
//    //*****

//    // получаем указатель на ячейку [row][col] ((!)нумерация с единицы)
//    QAxObject* cell = StatSheet->querySubObject("Cells(QVariant,QVariant)", row, col);
//    // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
//    cell->setProperty("Value", QVariant(data));
//    // получаем указатель на её фон
//    QAxObject* interior=cell->querySubObject("Interior");
//    // устанавливаем цвет
//    interior->setProperty("Color",QColor("green"));
//    // освобождение памяти
//    delete interior;

    int row = 1;
    int col = 1;
    // получение указателя на левую верхнюю ячейку [row][col] ((!)нумерация с единицы)
    QScopedPointer<QAxObject> cell1(sheet->querySubObject("Cells(QVariant&,QVariant&)", row, col));
    if(cell1.isNull())
    {
        emit toDebug(objectName(), "Cannot query Cells(QVariant&,QVariant&)");
        return false;
    }
    // получение указателя на правую нижнюю ячейку [row + numRows - 1][col + numCols - 1] ((!) numRows>=1,numCols>=1)
    QScopedPointer<QAxObject> cell2(sheet->querySubObject("Cells(QVariant&,QVariant&)",
                                                          row + data.size() - 1,
                                                          col + cols - 1));
    if(cell2.isNull())
    {
        emit toDebug(objectName(), "Cannot query Cells(QVariant&,QVariant&)");
        return false;
    }
    // получение указателя на целевую область
    QScopedPointer<QAxObject> range(sheet->querySubObject("Range(const QVariant&,const QVariant&)",
                                                          cell1->asVariant(), cell2->asVariant() ));
    if(range.isNull())
    {
        emit toDebug(objectName(), "Cannot query Cells(QVariant&,QVariant&)");
        return false;
    }
    // собственно вывод
    range->setProperty("Value", QVariant(data));

//    // Получение возможных команды
//    QFile outfile("excel_range.html");
//    outfile.open(QFile::WriteOnly | QFile::Truncate);
//    QTextStream out( &outfile );
//    QString docu = range->generateDocumentation();
//    out << docu;
//    outfile.close();

    // сохранение
    QString file(filename);
    file.replace("/", "\\");
    file.remove(".xlsx");
    file.remove(".xls");
    workbook->dynamicCall("SaveAs(const QString &)",
                          QVariant(file));

    // освобождение памяти
    range->clear();
    cell1->clear();
    cell2->clear();
    sheet->clear();
    sheets->clear();
    workbook->clear();
    workbooks->dynamicCall("Close()");
    workbooks->clear();
    excel->dynamicCall("Quit()");

    return true;
}


QString ExcelWidget::getCurrentTab()
{
    return _ui->_tabWidget->tabText(_ui->_tabWidget->currentIndex());
}

int ExcelWidget::getCountTab()
{
    return _ui->_tabWidget->count();
}

bool ExcelWidget::closeTab()
{
    QString sheet = getCurrentTab();
    if(sheet.isEmpty())
        return false;
    emit toDebug(objectName(),
                 QString("Закрытие вкладки '%1'").arg(sheet));
    emit finishSearchThread();
    QMessageBox msgBox(QMessageBox::Question,
                       trUtf8("Внимание"),
                       trUtf8("Вкладка '%1' будет закрыта."
                              "\nСохранить результаты перед закрытием?").arg(sheet));
    QPushButton *saveButton = msgBox.addButton(trUtf8("Cохранить"),
                                                  QMessageBox::AcceptRole);
    QPushButton *notSaveButton = msgBox.addButton(trUtf8("Не сохранять"),
                                                QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton(trUtf8("Отмена"),
                                                QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == saveButton) {
        emit toDebug(objectName(),
                     "Сохраняются результаты перед закрытием");
        if(!save())
            return true;
    } else if (msgBox.clickedButton() == notSaveButton) {
        emit toDebug(objectName(),
                     QString("Выбрано не сохранять результаты у вкладки '%1'").arg(sheet));
    } else if (msgBox.clickedButton() == cancelButton) {
        emit toDebug(objectName(),
                     QString("Отмена закрытия вкладки '%1'").arg(sheet));
        return true;
    }
    removeSheet(sheet);
    _ui->_tabWidget->removeTab(_ui->_tabWidget->currentIndex());
    emit toDebug(objectName(),
                 QString("Вкладка '%1' успешно закрыта").arg(sheet));
    emit messageReady(QString("Вкладка '%1' успешно закрыта").arg(sheet));

    if(_ui->_tabWidget->count()>0)
        return true;
    else
        return false;
}

void ExcelWidget::removeSheet(QString &sheet)
{
    _views.take(sheet)->deleteLater();
    _selections.take(sheet)->deleteLater();
    _models.take(sheet)->deleteLater();
    _mapHead.remove(sheet);
    _mapPHead.remove(sheet);
    _countParsedRow.remove(sheet);
    _countRow.remove(sheet);
    _editedRow.remove(sheet);
    _countRepatingRow.remove(sheet);
    _searchingRows.remove(sheet);
    _insertedRowAfterSearch.remove(sheet);
    _data2.remove(sheet);
}

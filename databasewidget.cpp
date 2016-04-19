#include "databasewidget.h"
#include "ui_databasewidget.h"


DatabaseWidget::DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DatabaseWidget),
    _csvFileName(DefaultBaseName),
    _thread(nullptr)
{
    _ui->setupUi(this);
    _db = new Database;
    _thread = new QThread;
    _db->moveToThread(_thread);
    _thread->start();

    connect(_db, SIGNAL(baseOpening()),
            this, SLOT(onBaseOpening()));
    connect(_db, SIGNAL(baseOpened()),
            this, SLOT(onBaseOpened()));

    connect(_db, SIGNAL(toDebug(QString,QString)),
             this, SIGNAL(toDebug(QString,QString)));

    connect(&_futureWatcher, SIGNAL(finished()),
            this, SLOT(onProcessOfOpenFinished()));
    connect(&_futureWatcherParser, SIGNAL(finished()),
            this, SLOT(onProcessOfParsingFinished()));

    _parser = new XlsParser;

    connect(_ui->comboBoxTypeFSubj, SIGNAL(activated(int)), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditFSubj, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditDistrict, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditTypeOfCity, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditTypeOfCity2, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditCity, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditCity2, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditTypeOfStreet, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditStreet, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditBuild, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditKorp, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditLiter, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));
    connect(_ui->lineEditAdditional, SIGNAL(returnPressed()), this, SLOT(onFindButtonClicked()));

    connect(_ui->_lineEditAddress, SIGNAL(returnPressed()), this, SLOT(onParseButtonClicked()));

    connect(this, SIGNAL(selectAddress(Address)),
            _db, SLOT(selectAddress(Address)));
    connect(_db, SIGNAL(selectedRows(int)),
            this, SLOT(onSelectedRows(int)));

    connect(this, SIGNAL(updateModel()),
            _db, SLOT(updateTableModel()));
    _ui->_pushButtonDelete->hide();
}

DatabaseWidget::~DatabaseWidget()
{
    qDebug() << " ~DatabaseWidget() <";
    delete _ui;
    _thread->quit();
//    _thread->wait();
    _db->deleteLater();
    _thread->deleteLater();
//    _paddr.take();
    qDebug() << " ~DatabaseWidget() >";
}


Database *DatabaseWidget::getDatabase()
{
    return _db;
}


void DatabaseWidget::readCsvBase(QString openFilename)
{
//    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug().noquote() << "readCsvBase BEGIN"
//             << currTime
//             << this->thread()->currentThreadId();

    emit toDebug(objectName(),
                 QString("Открывается файл '%1'.").arg(openFilename));

    QString name = QFileInfo(openFilename).fileName();
    if(!QFileInfo(openFilename).exists())
    {
        emit toDebug(objectName(),
                     QString("Файл '%1' не найден").arg(openFilename));
        return;
    }


    QProgressDialog *dialog = new QProgressDialog;
    dialog->setWindowTitle(trUtf8("Обработка базы (1/3)"));
    dialog->setLabelText(trUtf8("Открывается файл \"%1\". \nОжидайте ...")
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
    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     dialog, SLOT(hide()));

    QFuture<ListAddress> f1 = QtConcurrent::run(
                                         readFile,
                                         openFilename,
                                         MAX_OPEN_ROWS);
    // Start the computation.
    _futureWatcher.setFuture(f1);
    dialog->exec();

//    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//    qDebug().noquote() << "readCsvBase END"
//             << currTime
//             << this->thread()->currentThreadId();
}

ListAddress readFile(QString &openFilename, int maxCount)
{
    QFile file1(openFilename);
    if (!file1.open(QIODevice::ReadOnly))
        return ListAddress();
    ListAddress addrs;
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
            if(nRow>0)
            {
                Address a;
                a.setRawAddress(line);
                addrs.append(a);
            }
            nRow++;
        }
        //оставливаем обработку если получено нужное количество строк
        if(maxCount>0 && nRow >= maxCount)
            break;
    }
    file1.close();
    return addrs;
}

void parsingAddress(Address &a)
{
    QString line = a.getRawAddressString();
    a.setCorrect(false);
    int n1=line.indexOf('(');
    if (n1>0 && (line.indexOf(')',n1)>0))
    {
        int n2=line.indexOf(')', n1);
        int n3=n2-n1;
        a.setAdditional(line.mid(n1+1, n3-1));
        line.remove(n1, n3+1);
    }

    QRegExp reg(BaseRegPattern, Qt::CaseInsensitive);
    int pos=reg.indexIn(line);
    if(pos==-1)
    {
        a.setCorrect(false);
//        reg.setPattern(PatternForNotCorrect);
//        pos=reg.indexIn(line);
//        if(pos!=-1)
//        {
//            QStringList caps = reg.capturedTexts();
//            a.setBuildId(caps.at(3));
//            a.setStreetId(caps.at(1));
//        }
    }
    else
    {
        a.setCorrect(true);
        QStringList caps = reg.capturedTexts();
        a.setBuildId(caps.at(19));
        a.setStreetId(caps.at(1));
        a.setTypeOfFSubj(caps.at(4));
        a.setFsubj(caps.at(3));
        a.setDistrict(caps.at(7));
        a.setTypeOfCity1(caps.at(9));
        a.setCity1(caps.at(10));
        a.setTypeOfCity2(caps.at(12));
        a.setCity2(caps.at(13));
        a.setStreet(caps.at(16));
        a.setTypeOfStreet(caps.at(17));
        a.setBuild(caps.at(23));
        a.setKorp(caps.at(28));
        a.setLitera(caps.at(25)+caps.at(26)+caps.at(29));

//        if(a.getBuildId()==14042969)
//        {
//            int n=0;
//            foreach (QString s, caps) {
//                qDebug().noquote() << "[" << n << "]=" << s;
//                n++;
//            }
//            qDebug().noquote() << a.toString(PARSED);
//            assert(0);
//        }
    }
}

void DatabaseWidget::onProcessOfOpenFinished()
{
    if(_futureWatcher.isFinished()
            && !_futureWatcher.isCanceled())
    {
        _paddr.reset(new ListAddress(_futureWatcher.future().result()));
        emit toDebug(objectName(),
                     QString("Открытие файла успешно завершено. Прочитано строк = %1")
                     .arg(_paddr.data()->size()));
        if(!_paddr.data()->isEmpty())
        {
//                QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//                qDebug().noquote() << "onProcessOfOpenFinished BEGIN"
//                         << currTime
//                         << this->thread()->currentThreadId();

            emit toDebug(objectName(),
                         QString("Начало обработки прочитанной базы."));

            QProgressDialog *dialog = new QProgressDialog;
            dialog->setWindowTitle(trUtf8("Обработка базы (2/3)"));
            dialog->setLabelText(trUtf8("Обрабатывается файл. Строк: \"%1\". \nОжидайте ...")
                                 .arg(_paddr.data()->size()));
            dialog->setCancelButtonText(trUtf8("Отмена"));
            QObject::connect(dialog, SIGNAL(canceled()),
                             &_futureWatcherParser, SLOT(cancel()));
            QObject::connect(&_futureWatcherParser, SIGNAL(progressRangeChanged(int,int)),
                             dialog, SLOT(setRange(int,int)));
            QObject::connect(&_futureWatcherParser, SIGNAL(progressValueChanged(int)),
                             dialog, SLOT(setValue(int)));
            QObject::connect(&_futureWatcherParser, SIGNAL(finished()),
                             dialog, SLOT(deleteLater()));

            QFuture<void> f1 = QtConcurrent::map(*_paddr,
                                                 parsingAddress
                                                 );
            // Start the computation.
            _futureWatcherParser.setFuture(f1);
            dialog->exec();

//            currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//            qDebug().noquote() << "onProcessOfOpenFinished END"
//                               << currTime
//                               << this->thread()->currentThreadId();
        }
    }
    else
        emit toDebug(objectName(),
                     QString("Открытие файла завершено неудачей."));
}

void DatabaseWidget::onProcessOfParsingFinished()
{
    if(_futureWatcherParser.isFinished()
            && !_futureWatcherParser.isCanceled())
    {
        emit toDebug(objectName(),
                     QString("Обработка файла успешно завершена. Обработано строк = %1")
                     .arg(_paddr.data()->size()));

//        QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//        qDebug().noquote() << "Insert in Base BEGIN"
//                 << currTime
//                 << this->thread()->currentThreadId();

        QString name = QFileInfo(_db->baseName()).fileName();

        emit toDebug(objectName(),
                     QString("Добавление обработанных строк в базу '%1'.").arg(name));

        QProgressDialog *dialog = new QProgressDialog;
        QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>;
        dialog->setWindowTitle(trUtf8("Обработка базы (3/3)"));
        dialog->setLabelText(trUtf8("Добавляются строки в файл \"%1\". \nОжидайте ...")
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

        QFuture<void> f1 = QtConcurrent::run(_db,
                                             &Database::insertListAddressWithCheck,
                                             *_paddr);
        // Start the computation.
        futureWatcher->setFuture(f1);
        dialog->exec();

        if(dialog->wasCanceled())
        {
            emit toDebug(objectName(),
                         QString("Добавление отменено."));
            _db->cancelInsertOperation();
        }
        else if(f1.isFinished())
            emit toDebug(objectName(),
                         QString("Добавление завершено."));
        _paddr.reset();

//        currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
//        qDebug().noquote() << "Insert in Base END"
//                 << currTime
//                 << this->thread()->currentThreadId();
    }
    else
    {
        emit toDebug(objectName(),
                     QString("Обработка прочитанного файла завершена неудачей."));
    }
}

bool DatabaseWidget::open()
{
    QString fname =
            QFileDialog::getOpenFileName(this, trUtf8("Укажите файл базы данных"),
                                         "",
                                         tr("Excel (*.csv)"));
    if(fname.isEmpty())
    {
        return false;
    }
//    _ui->_lineEditFilename->setEnabled(true);
//    _csvFileName=fname;
//    _ui->_lineEditFilename->setText(fname);
//    clear();
//    _db->setBaseName(QString(fname).replace(".csv", ".db", Qt::CaseInsensitive));
    _db->setBaseName(QDir::current().filePath(DefaultBaseName));
    if(_db->removeBase(_db->baseName()))
    {
        _db->openBase(_db->baseName());

        readCsvBase(fname);
        //    _db->updateTableModel();
        emit updateModel();
        //    onBaseOpened();
        return true;
    }
    else
        return false;
}

void DatabaseWidget::openExisting(QString fname)
{
    _db->openBase(fname);
}

void DatabaseWidget::openExisting()
{
    if(QDir::current().exists(DefaultBaseName))
    {
        openExisting(QDir::current().filePath(DefaultBaseName));
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Warning,
                           trUtf8("Внимание"),
                           trUtf8("База данных '%1' в каталоге '%2' не найдена."
                                  "\nЗагрузить новую или найти старую?")
                           .arg(DefaultBaseName)
                           .arg(QDir::current().absolutePath()));
        QPushButton *openOldButton = msgBox.addButton(trUtf8("Указать старую"),
                                                      QMessageBox::AcceptRole);
        QPushButton *openNewButton = msgBox.addButton(trUtf8("Загрузить новую"),
                                                    QMessageBox::DestructiveRole);
        QPushButton *cancelButton = msgBox.addButton(trUtf8("Отмена"),
                                                    QMessageBox::RejectRole);
        msgBox.exec();
        if (msgBox.clickedButton() == openOldButton) {
            QString fname =
                    QFileDialog::getOpenFileName(this, trUtf8("Укажите файл базы данных"),
                                                 "",
                                                 tr("SQLite (*.db)"));
            if(fname.isEmpty())
            {
                emit exitSignal();
                return;
            }
            //копируем файл базы в текущий каталог (в каталог с программой)
            if(QFile::copy(fname, QDir::current().filePath(DefaultBaseName)))
            {
                //успешно скопировалось
                openExisting(QDir::current().filePath(DefaultBaseName));
            }
            else
            {
                openExisting(fname);
            }
        } else if (msgBox.clickedButton() == openNewButton) {
            if(!open())
                emit exitSignal();
        } else if (msgBox.clickedButton() == cancelButton) {
            emit exitSignal();
        }
    }
}

void DatabaseWidget::on__pushButtonOpen_clicked()
{
    open();
}

void DatabaseWidget::onBaseOpening()
{
    emit toDebug(objectName(),
                 QString("База данных '%1' открывается.").arg(_db->baseName()));
    _ui->_pushButtonOpen->setEnabled(false);
    _ui->_pushButtonFindParsAddr->setEnabled(false);
}

void DatabaseWidget::onBaseOpened()
{
    emit toDebug(objectName(),
                 QString("База данных '%1' была открыта.").arg(_db->baseName()));

//    _thread.wait(200);
//    qDebug() << "_db->getModel()" << _db->getModel();
    _ui->_tableView->setModel(_db->getModel());
    emit updateModel();

    _ui->_pushButtonOpen->setEnabled(true);
    _ui->_pushButtonFindParsAddr->setEnabled(true);
}

void DatabaseWidget::on__pushButtonFindParsAddr_clicked()
{
    onFindButtonClicked();
}

void DatabaseWidget::onFindButtonClicked()
{
    Address a;
    if(_ui->comboBoxTypeFSubj->currentText().isEmpty())
        a.setTypeOfFSubj(MapFSubjString[INCORRECT_SUBJ]);
    else
        a.setTypeOfFSubj(_ui->comboBoxTypeFSubj->currentText());
    a.setFsubj(_ui->lineEditFSubj->text());
    a.setDistrict(_ui->lineEditDistrict->text());
    a.setTypeOfCity1(_ui->lineEditTypeOfCity->text());
    a.setTypeOfCity2(_ui->lineEditTypeOfCity2->text());
    a.setCity1(_ui->lineEditCity->text());
    a.setCity2(_ui->lineEditCity2->text());
    a.setTypeOfStreet(_ui->lineEditTypeOfStreet->text());
    a.setStreet(_ui->lineEditStreet->text());
    a.setBuild(_ui->lineEditBuild->text());
    a.setKorp(_ui->lineEditKorp->text());
    a.setLitera(_ui->lineEditLiter->text());
    a.setAdditional(_ui->lineEditAdditional->text());
    a.setBuildId(_ui->lineEditBuildId->text());
    a.setStreetId(_ui->lineEditStreetId->text());
    a.setCorrect((bool)_ui->checkBoxCorrect->checkState());

    emit toDebug(objectName(),
                 QString("Произодим поиск след. адреса(-ов): %1").arg(a.toCsv()));
    emit selectAddress(a);
}

void DatabaseWidget::on__pushButtonParseAddr_clicked()
{
    onParseButtonClicked();
}

void DatabaseWidget::onParseButtonClicked()
{
    QString addr(_ui->_lineEditAddress->text());
    emit toDebug(objectName(),
                 QString("Распознаем адрес для поиска: %1").arg(addr));
    Address a;
    _parser->parseAddress(addr, a);

    emit toDebug(objectName(),
                 QString("Адрес распознан: %1").arg(a.toCsv()));
    QString str;
    str=a.getTypeOfFSubjInString();
    int index = _ui->comboBoxTypeFSubj->findText(str, Qt::MatchExactly);
    if(index != -1)
        _ui->comboBoxTypeFSubj->setCurrentIndex(index);
    else
        _ui->comboBoxTypeFSubj->setCurrentIndex(0);
    str=a.getFsubj();
    _ui->lineEditFSubj->setText(str);
    str=a.getDistrict();
    _ui->lineEditDistrict->setText(str);
    str=a.getTypeOfCity1();
    _ui->lineEditTypeOfCity->setText(str);
    str=a.getCity1();
    _ui->lineEditCity->setText(str);
    str=a.getTypeOfCity2();
    _ui->lineEditTypeOfCity2->setText(str);
    str=a.getCity2();
    _ui->lineEditCity2->setText(str);
    str=a.getAdditional();
    _ui->lineEditAdditional->setText(str);
    str=a.getTypeOfStreet();
    _ui->lineEditTypeOfStreet->setText(str);
    str=a.getStreet();
    _ui->lineEditStreet->setText(str);
    str=a.getBuild();
    _ui->lineEditBuild->setText(str);
    str=a.getKorp();
    _ui->lineEditKorp->setText(str);
    str=a.getLitera();
    _ui->lineEditLiter->setText(str);

    _ui->_pushButtonFindParsAddr->setFocus();
}


void DatabaseWidget::onSelectedRows(int count)
{
    emit toDebug(objectName(),
                 QString("Получено строк в модели = '%1'").arg(count));
    QString str;
    if(count==256)
        str=">256";
    else
        str=QString::number(count);
    _ui->_labelCountRows->setText(str);
}

void DatabaseWidget::on__pushButtonClear_clicked()
{
    onClearButtonClicked();
}

void DatabaseWidget::onClearButtonClicked()
{
    QString str("*");
    _ui->comboBoxTypeFSubj->setCurrentIndex(0);
    _ui->lineEditFSubj->setText(str);
    _ui->lineEditDistrict->setText(str);
    _ui->lineEditTypeOfCity->setText(str);
    _ui->lineEditCity->setText(str);
    _ui->lineEditTypeOfCity2->setText(str);
    _ui->lineEditCity2->setText(str);
    _ui->lineEditAdditional->setText(str);
    _ui->lineEditTypeOfStreet->setText(str);
    _ui->lineEditStreet->setText(str);
    _ui->lineEditBuild->setText(str);
    _ui->lineEditKorp->setText(str);
    _ui->lineEditLiter->setText(str);
    _ui->lineEditBuildId->setText(str);
    _ui->lineEditStreetId->setText(str);
    _ui->checkBoxCorrect->setChecked(true);

    _ui->_pushButtonFindParsAddr->setFocus();
}

void DatabaseWidget::on__pushButtonDelete_clicked()
{
    _db->removeBase(_db->baseName());
}

#include "databasewidget.h"
#include "ui_databasewidget.h"

DatabaseWidget::DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DatabaseWidget),
    _csvFileName(DefaultBaseName)
{
    _ui->setupUi(this);
    _db = new Database;
    _db->moveToThread(&_thread);
    _thread.start();

    _ui->_progressBarReaded->hide();

    openExisting();

    connect(_db, SIGNAL(countRows(int)),
            this, SLOT(onCountRow(int)));
    connect(_db, SIGNAL(rowParsed(int)),
            this, SLOT(onParseRow(int)));
    connect(_db, SIGNAL(rowReaded(int)),
            this, SLOT(onReadRow(int)));
    connect(_db, SIGNAL(readedRows(int)),
            this, SLOT(onReadRows(int)));
    connect(_db, SIGNAL(workingWithOpenBase()),
            this, SLOT(onOpenBase()));
    connect(_db, SIGNAL(baseOpened()),
            this, SLOT(onBaseOpened()));
    connect(_db, SIGNAL(toDebug(QString,QString)),
             SIGNAL(toDebug(QString,QString)));

    connect(&_futureWatcher, SIGNAL(finished()),
            this, SLOT(onProcessOfOpenFinished()));
    connect(&_futureWatcherParser, SIGNAL(finished()),
            this, SLOT(onProcessOfParsingFinished()));
//    _ui->_pushButtonLoadOld->hide();
}

DatabaseWidget::~DatabaseWidget()
{
    delete _ui;
    delete _db;
    _thread.quit();
    _thread.wait();
}

void DatabaseWidget::readCsvBase(QString openFilename)
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug().noquote() << "readCsvBase BEGIN"
             << currTime
             << this->thread()->currentThreadId();

    QString name = QFileInfo(openFilename).fileName();

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

    CsvWorker *csv = new CsvWorker;
    QObject::connect(&_futureWatcher, SIGNAL(finished()),
                     csv, SLOT(deleteLater()));

    QFuture<ListAddress> f1 = QtConcurrent::run(csv,
                                         &CsvWorker::readFile,
                                         openFilename,
                                         MAX_OPEN_ROWS);
    // Start the computation.
    _futureWatcher.setFuture(f1);
    dialog->exec();

    currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    qDebug().noquote() << "readCsvBase END"
             << currTime
             << this->thread()->currentThreadId();
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
        _addrs = _futureWatcher.future().result();
        emit toDebug(objectName(),
                     QString("finish open good, size=%1")
                     .arg(QString::number(_addrs.size()))
                     );
        if(!_addrs.isEmpty())
        {
                QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
                qDebug().noquote() << "onProcessOfOpenFinished BEGIN"
                         << currTime
                         << this->thread()->currentThreadId();

                QProgressDialog *dialog = new QProgressDialog;
                dialog->setWindowTitle(trUtf8("Обработка базы (2/3)"));
                dialog->setLabelText(trUtf8("Обрабатывается файл. Строк: \"%1\". \nОжидайте ...")
                                     .arg(_addrs.size()));
                dialog->setCancelButtonText(trUtf8("Отмена"));
                QObject::connect(dialog, SIGNAL(canceled()),
                                 &_futureWatcherParser, SLOT(cancel()));
                QObject::connect(&_futureWatcherParser, SIGNAL(progressRangeChanged(int,int)),
                                 dialog, SLOT(setRange(int,int)));
                QObject::connect(&_futureWatcherParser, SIGNAL(progressValueChanged(int)),
                                 dialog, SLOT(setValue(int)));
                QObject::connect(&_futureWatcherParser, SIGNAL(finished()),
                                 dialog, SLOT(deleteLater()));

                QFuture<void> f1 = QtConcurrent::map(_addrs,
                                                     parsingAddress
                                                     );
                // Start the computation.
                _futureWatcherParser.setFuture(f1);
                dialog->exec();

                currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
                qDebug().noquote() << "onProcessOfOpenFinished END"
                         << currTime
                         << this->thread()->currentThreadId();
        }
    }
    else
        emit toDebug(objectName(),QString("finish open bad"));
}

void DatabaseWidget::onProcessOfParsingFinished()
{
    if(_futureWatcherParser.isFinished()
            && !_futureWatcherParser.isCanceled())
    {
        emit toDebug(objectName(),
                     QString("finish parsing good, size=%1")
                     .arg(QString::number(_addrs.size()))
                     );

        QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
        qDebug().noquote() << "Insert in Base BEGIN"
                 << currTime
                 << this->thread()->currentThreadId();

        QString name = QFileInfo(_db->baseName()).fileName();

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
                                             _addrs);
        // Start the computation.
        futureWatcher->setFuture(f1);
        dialog->exec();

        currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
        qDebug().noquote() << "Insert in Base END"
                 << currTime
                 << this->thread()->currentThreadId();
    }
    else
    {
        emit toDebug(objectName(),
                     QString("finish parsing bad, size=%1")
                     .arg(QString::number(_addrs.size()))
                     );
    }
}

void DatabaseWidget::open()
{
    QString fname =
            QFileDialog::getOpenFileName(this, trUtf8("Укажите файл базы данных"),
                                         "",
                                         tr("Excel (*.csv)"));
    if(fname.isEmpty())
        return;
    _ui->_lineEditFilename->setEnabled(true);
    _csvFileName=fname;
    _ui->_lineEditFilename->setText(fname);
    clear();
    _db->setBaseName(QString(fname).replace(".csv", ".db", Qt::CaseInsensitive));
    _db->removeBase(_db->baseName());
    _db->openBase(_db->baseName());

    readCsvBase(fname);
    _db->updateTableModel();
}

void DatabaseWidget::openExisting()
{
    QStringList files = QDir::current().entryList(QStringList("*.db"), QDir::Files);
    emit toDebug(objectName(), "databases:"+files.join(" "));
    if(files.size()>1 || files.isEmpty())
    {
        QString fname =
                QFileDialog::getOpenFileName(this, trUtf8("Укажите файл базы данных"),
                                             "",
                                             tr("SQLite (*.db)"));
        if(fname.isEmpty())
            return;
        _db->openBase(fname);
    }
    else
    {
        QString fname = files.first();
        _db->openBase(fname);
    }

}

void DatabaseWidget::clear()
{

}

void DatabaseWidget::waitSearch()
{
    _thread.start();
}

void DatabaseWidget::viewInfo()
{

}

void DatabaseWidget::on__pushButtonOpen_clicked()
{
    open();
}

void DatabaseWidget::on__pushButtonLoadOld_clicked()
{
    openExisting();
}

void DatabaseWidget::onReadRow(int row)
{
    _ui->_progressBarReaded->setValue(row+1);
}

void DatabaseWidget::onReadRows(int rows)
{
    _ui->_progressBarReaded->setMaximum(rows);
    _ui->_progressBarParsed->setMaximum(rows);
}

void DatabaseWidget::onParseRow(int row)
{
    _ui->_progressBarParsed->setValue(row+1);
}

void DatabaseWidget::onCountRow(int count)
{
    _ui->_progressBarReaded->reset();
    _ui->_progressBarReaded->setMaximum(count);
    _ui->_progressBarParsed->setMaximum(count);
}


void DatabaseWidget::onOpenBase()
{
    _ui->_pushButtonOpen->setEnabled(false);
}

void DatabaseWidget::onBaseOpened()
{
    _ui->_pushButtonOpen->setEnabled(true);
    _ui->_pushButtonLoadOld->setEnabled(true);

    connectModelWithView(_db->getModel());

    _ui->_progressBarReaded->setMaximum(1);
    _ui->_progressBarParsed->setMaximum(1);
    _ui->_progressBarReaded->setValue(1);
    _ui->_progressBarParsed->setValue(1);
}

void DatabaseWidget::connectModelWithView(QSqlTableModel *model)
{
    _ui->_tableView->setModel(model);
//    _ui->_tableView->hideColumn(0);
    _ui->_tableView->hideColumn(model->columnCount()-1);
}

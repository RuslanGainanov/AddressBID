#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    int id = qRegisterMetaType< Address >("Address");
    id = qRegisterMetaType< QMap<QString,int> >("QMap<QString,int>");
    id = qRegisterMetaType< MapAddressElementPosition >("MapAddressElementPosition");
    id = qRegisterMetaType< ExcelSheet >("ExcelSheet");
    id = qRegisterMetaType< ExcelDocument >("ExcelDocument");
    id = qRegisterMetaType< ListAddress >("ListAddress");
    Q_UNUSED(id);
    _dbw = new DatabaseWidget;
    _excel = _ui->_excelWidget;
//    _debugWidget =_ui->_debugWidget;
    _debugWidget = new DebugWidget;
    _excel->setObjectName("ExcelWidget");

    _helpBrowser = new HelpBrowser(":/doc", "index.htm");

    connect(this, SIGNAL(toDebug(QString,QString)),
            _debugWidget, SLOT(add(QString,QString)));

    connect(_ui->_actionOpenBase, SIGNAL(triggered()),
            this, SLOT(onBaseOpenTriggered()));
    connect(_ui->_actionOpenFile, SIGNAL(triggered()),
            this, SLOT(onExcelOpenTriggered()));

    connect(_dbw, SIGNAL(toDebug(QString,QString)),
            _debugWidget, SLOT(add(QString,QString)));
    connect(_excel, SIGNAL(toDebug(QString,QString)),
            _debugWidget, SLOT(add(QString,QString)));
    connect(_excel, SIGNAL(messageReady(QString)),
            this, SLOT(onMessageReady(QString)));

    connect(_excel, SIGNAL(errorOccured(QString,int,QString)),
            this, SLOT(onErrorOccured(QString,int,QString)));

    connect(_ui->_pushButtonSave, SIGNAL(clicked()),
            this, SLOT(save()));

//    connect(_dbw, SIGNAL(exitSignal()),
//            this, SLOT(close()));
//    connect(_dbw, SIGNAL(exitSignal()),
//            qApp, SLOT(quit()));
    connect(_dbw, SIGNAL(exitSignal()),
            SIGNAL(windowClosed()));
    connect( this, SIGNAL(windowClosed()),
             _dbw, SLOT(close()) );
//    connect( this, SIGNAL(windowClosed()),
//             _excel, SLOT(close()) );
    connect( this, SIGNAL(windowClosed()),
             _helpBrowser, SLOT(close()) );

//    Database *db = _dbw->getDatabase();
//    connect(_excel, SIGNAL(searchInBase(QString,int,Address)),
//            db, SLOT(selectAddress(QString,int,Address)));
//    connect(db, SIGNAL(addressFounded(QString,int,Address)),
//            _excel, SLOT(onFounedAddress(QString,int,Address)));
//    connect(db, SIGNAL(addressNotFounded(QString,int,Address)),
//            _excel, SLOT(onNotFounedAddress(QString,int,Address)));

    connect(_excel, &ExcelWidget::searching,
            this, &MainWindow::onStartSearching);
    connect(_excel, &ExcelWidget::searchFinished,
            this, &MainWindow::onFinishSearching);
    connect(_excel, &ExcelWidget::sheetParsed,
            this, &MainWindow::onSheetParsed);
    connect(_excel, &ExcelWidget::opening,
            this, &MainWindow::onFileOpening);
    connect(_excel, &ExcelWidget::openFinished,
            this, &MainWindow::onFileOpened);
    connect(_excel, &ExcelWidget::openError,
            this, &MainWindow::onFileError);

    _debugWidget->hide();
    _ui->_pushButtonOpenBase->hide();
    _ui->_progressBar->hide();
    _ui->_pushButtonWait->hide();
    _ui->_pushButtonStop->hide();

    connect(_ui->_actionAbout, SIGNAL(triggered(bool)),
            this, SLOT(showAbout()));
    connect(_ui->_actionHelp, SIGNAL(triggered(bool)),
            this, SLOT(showHelp()));

//    _dbw->show();
    _dbw->openExisting();
//    qDebug() << "end constructor MainWindow";
}

MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow()";
    delete _ui;
    delete _dbw;
    delete _helpBrowser;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closeEvent()";
    emit windowClosed();
    QWidget::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *pe)
{
    switch(pe->key())
    {
    case Qt::Key_F1:
        showHelp();
        break;
    default:
        QWidget::keyPressEvent(pe);
        break;
    }
}

void MainWindow::show()
{
//    _dbw->openExisting();
    QMainWindow::show();
//    _dbw->show();
}

void MainWindow::showAbout()
{
    QString text;
    text.append(tr("<p><h2>Обработчик тендерных заявок</h2></p>"));
    text.append(tr("<p><h3>Программа ищет соответвие адресов из заявки в Excel-таблице с адресами в базе данных и производит назначение STREET_ID и BUILD_ID найденным адресам.</h3></p>"));
    text.append(tr("<p>Программу разработал <a href='http://gromr1.blogspot.ru/p/about-me.html'>Гайнанов Руслан</a>.</p>"));
    text.append(tr("<p>Связаться с автором: <a href='mailto:ruslan.r.gainanov@gmail.com'>ruslan.r.gainanov@gmail.com</a>.</p>"));
    text.append(tr("<p><br /><small>© Все права принадлежат компании <a href='rt.ru'>Ростелеком</a>.</small></p>"));
    QMessageBox::about(this, tr("О программе"), text);
}

void MainWindow::showHelp()
{
    _helpBrowser->show();
}

void MainWindow::onBaseOpenTriggered()
{
    _dbw->show();
}

void MainWindow::onExcelOpenTriggered()
{
    _excel->open();
}

void MainWindow::onErrorOccured(QString nameObject, int code, QString errorDesc)
{
    Q_UNUSED(nameObject);
    Q_UNUSED(code);
    _ui->statusBar->showMessage(errorDesc, 5000);
}

void MainWindow::onMessageReady(QString mes)
{
    _ui->statusBar->showMessage(mes, 4000);
    emit toDebug(objectName(),
                 "MR: "+mes);
}

void MainWindow::on__pushButtonOpen_clicked()
{
    _excel->open();
}

void MainWindow::on__pushButtonOpenBase_clicked()
{
    _dbw->show();
}

void MainWindow::on__pushButtonSearch_clicked()
{
    _excel->search();
//    _excel->search2();
}

void MainWindow::onStartSearching(const QString &sheet)
{
    _ui->_pushButtonSearch->setEnabled(false);
    _ui->_pushButtonStop->setEnabled(true);
    _ui->_pushButtonWait->setEnabled(true);
    QString mes=QString("Поиск в документе '%1' начат").arg(sheet);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onFinishSearching(const QString &sheet)
{
    if(_excel->getCountTab()>0)
        _ui->_pushButtonSearch->setEnabled(true);
    else
        _ui->_pushButtonSearch->setEnabled(false);
    _ui->_pushButtonStop->setEnabled(false);
    _ui->_pushButtonWait->setEnabled(false);
    QString mes=QString("Поиск в документе '%1' завершен").arg(sheet);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onSheetParsed(const QString &sheet)
{
    _ui->_pushButtonSearch->setEnabled(true);
    QString mes=QString("Обработка документа '%1' завершена").arg(sheet);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onFileOpening(const QString &fname)
{
    _ui->_pushButtonSearch->setEnabled(false);
    _ui->_pushButtonOpen->setEnabled(false);
    QString name = QFileInfo(fname).fileName();
    QString mes=QString("Документ '%1' открывается").arg(name);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onFileError(const QString &fname)
{
//    _ui->_pushButtonSearch->setEnabled(false);
    _ui->_pushButtonOpen->setEnabled(true);
    QString name = QFileInfo(fname).fileName();
    QString mes=QString("Документ '%1' не открывается").arg(name);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onFileOpened(const QString &fname)
{
    _ui->_pushButtonSearch->setEnabled(false);
    _ui->_pushButtonOpen->setEnabled(true);
    _ui->_pushButtonSave->setEnabled(true);
    _ui->_pushButtonCloseTab->setEnabled(true);
    _excel->getParseWidget()->setEnabled(true);
    QString name = QFileInfo(fname).fileName();
    QString mes=QString("Документ '%1' успешно открыт. Начата его обработка").arg(name);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::save()
{
    _excel->save();
}

void MainWindow::on__pushButtonWait_clicked()
{
//    QThread::yieldCurrentThread();
    _excel->waitSearch();
}

void MainWindow::on__pushButtonCloseTab_clicked()
{
    if(_excel->closeTab())
    {
        _ui->_pushButtonCloseTab->setEnabled(true);
        _ui->_pushButtonSave->setEnabled(true);
    }
    else
    {
        //если закрыли последнюю вкладку
        _ui->_pushButtonCloseTab->setEnabled(false);
        _ui->_pushButtonSave->setEnabled(false);
        _ui->_pushButtonSearch->setEnabled(false);
        _excel->getParseWidget()->setEnabled(false);

        _ui->_pushButtonOpen->setFocus();
    }
}

void MainWindow::on__pushButtonStop_clicked()
{
    _excel->stopSearch();
}

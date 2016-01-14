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
    Database *db = _dbw->getDatabase();
    _excel = _ui->_excelWidget;

    connect(this, SIGNAL(toDebug(QString,QString)),
            _ui->_debugWidget, SLOT(add(QString,QString)));

    connect(_ui->_actionOpenBase, SIGNAL(triggered()),
            this, SLOT(onBaseOpenTriggered()));
    connect(_ui->_actionOpenFile, SIGNAL(triggered()),
            this, SLOT(onExcelOpenTriggered()));

    connect(_dbw, SIGNAL(toDebug(QString,QString)),
            _ui->_debugWidget, SLOT(add(QString,QString)));
    connect(_excel, SIGNAL(toDebug(QString,QString)),
            _ui->_debugWidget, SLOT(add(QString,QString)));
    connect(_excel, SIGNAL(messageReady(QString)),
            this, SLOT(onMessageReady(QString)));

    connect(_excel, SIGNAL(errorOccured(QString,int,QString)),
            this, SLOT(onErrorOccured(QString,int,QString)));

    connect(_ui->_pushButtonSave, SIGNAL(clicked()),
            this, SLOT(save()));

    connect( this, SIGNAL(windowClosed()),
             _dbw, SLOT(close()) );
    connect( this, SIGNAL(windowClosed()),
             _excel, SLOT(close()) );

    connect(_excel, SIGNAL(findRowInBase(QString,int,Address)),
            db, SLOT(selectAddress(QString,int,Address)));
    connect(db, SIGNAL(addressFounded(QString,int,Address)),
            _excel, SLOT(onFounedAddress(QString,int,Address)));
    connect(db, SIGNAL(addressNotFounded(QString,int,Address)),
            _excel, SLOT(onNotFounedAddress(QString,int,Address)));


    connect(_excel, &ExcelWidget::searching,
            this, &MainWindow::onStartSearching);
    connect(_excel, &ExcelWidget::searchFinished,
            this, &MainWindow::onFinishSearching);

//        _dbw->openExisting();
//        _dbw->show();
    _ui->_debugWidget->hide();
    _ui->_pushButtonOpenBase->hide();
    _ui->_progressBar->hide();
    setWindowTitle(trUtf8("RT: Обработчик тендерных заявок"));
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _dbw;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
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
}

void MainWindow::onStartSearching(const QString &sheet)
{
    _ui->_pushButtonSearch->setEnabled(false);
    QString mes=QString("Поиск в документе '%1' начат").arg(sheet);
    _ui->statusBar->showMessage(mes, 3000);
    emit toDebug(objectName(),
                 mes);
}

void MainWindow::onFinishSearching(const QString &sheet)
{
    _ui->_pushButtonSearch->setEnabled(true);
    QString mes=QString("Поиск в документе '%1' завершен").arg(sheet);
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
    QThread::yieldCurrentThread();
}

void MainWindow::on__pushButtonCloseTab_clicked()
{
    _excel->closeTab();
}

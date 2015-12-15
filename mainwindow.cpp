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
    _dbw->openExisting();
    _excel = _ui->_excelWidget;

    connect(_ui->_actionOpenBase, SIGNAL(triggered()),
            this, SLOT(onBaseOpenTriggered()));
    connect(_ui->_actionOpenFile, SIGNAL(triggered()),
            this, SLOT(onExcelOpenTriggered()));

    connect(_dbw, SIGNAL(toDebug(QString)),
            _ui->_debugWidget, SLOT(add(QString)));
    connect(_excel, SIGNAL(toDebug(QString,QString)),
            _ui->_debugWidget, SLOT(add(QString,QString)));

    connect(_excel, SIGNAL(errorOccured(QString,int,QString)),
            this, SLOT(onErrorOccured(QString,int,QString)));

    connect( this, SIGNAL(windowClosed()),
             _dbw, SLOT(close()) );
    connect( this, SIGNAL(windowClosed()),
             _excel, SLOT(close()) );

    _ui->_debugWidget->hide();
    _ui->_pushButtonOpenBase->hide();
    _ui->_pushButtonDeleteRows->hide();
    _ui->_pushButtonCheckRows->hide();
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
    Database *db=_dbw->getDatabase();
    _excel->setDatabase(db);
    _excel->search();
}

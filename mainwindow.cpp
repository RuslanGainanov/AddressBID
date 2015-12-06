#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _dbw = new DatabaseWidget;
    _excel = new ExcelWidget;
    int id = qRegisterMetaType< Address >("Address");
    qDebug() << "qRegisterMetaType< Address >(\"Address\")"
             << id;
    id = qRegisterMetaType< QMap<QString,int> >("QMap<QString,int>");
    qDebug() << "qRegisterMetaType< QMap<QString,int> >(\"QMap<QString,int>\")"
             << id;
    id = qRegisterMetaType< MapAddressElementPosition >("MapAddressElementPosition");
    qDebug() << "qRegisterMetaType< MapAddressElementPosition >(\"MapAddressElementPosition\")"
             << id;

    connect(_ui->_actionOpenBase, SIGNAL(triggered()),
            this, SLOT(onBaseOpenTriggered()));
    connect(_ui->_actionOpenFile, SIGNAL(triggered()),
            this, SLOT(onExcelOpenTriggered()));
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _dbw;
    delete _excel;
}

void MainWindow::onBaseOpenTriggered()
{
    _dbw->show();
}

void MainWindow::onExcelOpenTriggered()
{
    _excel->show();
}

void MainWindow::on__pushButtonOpen_clicked()
{
    _excel->show();
}

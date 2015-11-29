#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _dbw = new DatabaseWidget(this);
    int id = qRegisterMetaType< Address >("Address");
    qDebug() << "qRegisterMetaType< Address >(\"Address\")"
             << id;
    id = qRegisterMetaType< QMap<QString,int> >("QMap<QString,int>");
    qDebug() << "qRegisterMetaType< QMap<QString,int> >(\"QMap<QString,int>\")"
             << id;
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _dbw;
}

void MainWindow::onBaseOpenClicked()
{
    _dbw->show();
}

#include "databasewidget.h"
#include "ui_databasewidget.h"

DatabaseWidget::DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DatabaseWidget)
{
    _ui->setupUi(this);
    _db = new Database(this);

    _ui->_progressBarReaded->hide();

    if(QFile("Base_2.db").exists())
        _ui->_pushButtonLoadOld->setEnabled(true);
    else
        _ui->_pushButtonLoadOld->setEnabled(false);
    _db->setBaseName("Base_2.db");

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
    connect(_db, SIGNAL(toDebug(QString)),
             SIGNAL(toDebug(QString)));
    connect(_db, SIGNAL(messageReady(QString)),
            SIGNAL(toDebug(QString)));

    _ui->_pushButtonLoadOld->hide();
}

DatabaseWidget::~DatabaseWidget()
{
    delete _ui;
    delete _db;
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
    _ui->_lineEditFilename->setText(fname);
    clear();
    _db->openBase(fname);
}

void DatabaseWidget::openExisting()
{
    _db->openOldBase();
}

void DatabaseWidget::clear()
{

}


void DatabaseWidget::viewInfo()
{

}

void DatabaseWidget::on__pushButtonOpen_clicked()
{
    open();
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

void DatabaseWidget::on__pushButtonLoadOld_clicked()
{
    _db->openOldBase();
}


void DatabaseWidget::connectModelWithView(QSqlTableModel *model)
{
    _ui->_tableView->setModel(model);
    _ui->_tableView->hideColumn(0);
    _ui->_tableView->hideColumn(model->columnCount()-1);
}

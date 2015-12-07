#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _thread(nullptr),
    _parser(nullptr),
    _xls(nullptr)
{
    _ui->setupUi(this);
    _thread = new QThread;
}

ExcelWidget::~ExcelWidget()
{
    delete _ui;
    if(_thread)
    {
        _thread->quit();
        _thread->wait();
        delete _thread;
    }
}

void ExcelWidget::on__pushButtonOpen_clicked()
{
    open();
}

void ExcelWidget::open()
{
    QString str =
            QFileDialog::getOpenFileName(this,
                                         trUtf8("Укажите исходный файл"),
                                         "",
                                         tr("Excel (*.xls *.xlsx)"));
    if(str.isEmpty())
        return;
    _ui->_lineEditFilename->setText(str);
    runThread(str);
}


void ExcelWidget::runThread(QString openFilename)
{
    qDebug() << "ExcelWidget runThread" << this->thread()->currentThreadId();
    if(_thread->isRunning())
        return;

    _xls = new XlsWorker;
    _xls->setFileName(openFilename);
    _xls->setMaxCountRead(MAX_OPEN_ROWS);
    _xls->moveToThread(_thread);
    connect(_xls, SIGNAL(toDebug(QString)),
            this, SIGNAL(toDebug(QString)));
    connect(_thread, SIGNAL(started()),
            _xls, SLOT(process()));
    connect(_xls, SIGNAL(countRows(int)),
            this, SLOT(onCountRows(int)));
    connect(_xls, SIGNAL(rowReaded(int,int,QStringList)),
            this, SLOT(onReadRow(int,QStringList)));//***
    connect(_xls, SIGNAL(rowsReaded(int,int)),
            this, SLOT(onReadRows(int)));//***
    connect(_xls, SIGNAL(finished()),
            this, SLOT(onFinishCsvWorker()));

    _parser = new Parser;
    _parser->setTypeOfRow(INPUT);
    _parser->moveToThread(_thread);
    connect(_xls, SIGNAL(headReaded(QStringList)),
            _parser, SLOT(onReadHeadBase(QStringList)));
    connect(_xls, SIGNAL(rowReaded(int,QStringList)),
            _parser, SLOT(onReadRow(int,QStringList)));
    connect(_parser, SIGNAL(rowParsed(int, Address)),
            this, SLOT(onParseRow(int, Address)));

    connect(_thread, SIGNAL(finished()),
            _parser, SLOT(deleteLater()));
    connect(_thread, SIGNAL(finished()),
            _xls, SLOT(deleteLater()));
    _thread->start();

    emit workingWithOpenBase();
}

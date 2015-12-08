#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget),
    _xls(nullptr),
    _thread(nullptr),
    _parser(nullptr),
    _xlsParser(nullptr)
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
    _xls->setMaxCountRead(MAX_OPEN_IN_ROWS);
    _xlsParser = new XlsParser;
    _xlsParser->moveToThread(_thread);
    connect(_xls, SIGNAL(toDebug(QString,QString)),
            this, SIGNAL(toDebug(QString,QString)));
    connect(_xls, SIGNAL(sheetsReaded(QMap<int,QString>)),
            this, SLOT(onSheetsReaded(QMap<int,QString>)));
    connect(_xls, SIGNAL(errorOccured(QString,int,QString)),
            this, SIGNAL(errorOccured(QString,int,QString)));
    connect(_xls, SIGNAL(countRows(int,int)),
            this, SLOT(onCountRows(int,int)));
    connect(_xls, SIGNAL(headReaded(int,QStringList)),
            this, SLOT(onReadHead(int,QStringList)));
    connect(_xls, SIGNAL(headReaded(int,QStringList)),
            _xlsParser, SLOT(onReadHead(int,QStringList)));
    connect(_xls, SIGNAL(rowReaded(int,int,QStringList)),
            this, SLOT(onReadRow(int,int,QStringList)));
    connect(_xls, SIGNAL(rowReaded(int,int,QStringList)),
            _xlsParser, SLOT(onReadRow(int,int,QStringList)));
    connect(_xls, SIGNAL(rowsReaded(int,int)),
            this, SLOT(onReadRows(int,int)));
    connect(_xls, SIGNAL(finished()),
            this, SLOT(onFinishWorker()));
    connect(_xlsParser, SIGNAL(finished()),
            this, SLOT(onFinishParser()));
    connect(_xlsParser, SIGNAL(rowParsed(int,int,Address)),
            this, SLOT(onParseRow(int,int,Address)));
    connect(_xlsParser, SIGNAL(headParsed(int,MapAddressElementPosition)),
            this, SLOT(onHeadParsed(int,MapAddressElementPosition)));

    connect(_thread, SIGNAL(finished()),
            _xlsParser, SLOT(deleteLater()));
    connect(_thread, SIGNAL(finished()),
            _xls, SLOT(deleteLater()));

    _thread->start();

    _xls->process();

    emit working();
}

void ExcelWidget::onReadHead(int sheet, QStringList head)
{
    emit toDebug(objectName(),
                 QString("Прочитана шапка у листа №%1").arg(sheet));
    _sheetsHead.insert(sheet, head);
    emit headReaded(sheet, head);
}

void ExcelWidget::onReadRow(int sheet, int row, QStringList listRow)
{
    Q_UNUSED(listRow);
    emit toDebug(objectName(), "ExcelWidget::onReadRow");
    emit rowReaded(sheet, row);
}

void ExcelWidget::onReadRows(int sheet, int count)
{
    emit readedRows(sheet, count);
}

void ExcelWidget::onCountRows(int sheet, int count)
{
    emit countRows(sheet, count);
}

void ExcelWidget::onHeadReaded(int sheet, QStringList head)
{
    emit headReaded(sheet, head);
}

void ExcelWidget::onSheetsReaded(const QMap<int, QString> &sheets)
{
    emit toDebug(objectName(),
                 "В документе есть следующие лист(-ы) с информацией:\r\n"
                 +QStringList(sheets.values()).join(", "));
    _sheets=sheets;
    emit sheetsReaded(sheets);
}

void ExcelWidget::onFinishWorker()
{
    _thread->quit();
    _thread->wait();
    emit toDebug(objectName(),
                 "ExcelWidget::onFinishWorker()"
                 );
    emit finished();
}

void ExcelWidget::onHeadParsed(int sheet, MapAddressElementPosition head)
{
    emit toDebug(objectName(),
                 "ExcelWidget onHeadParsed "
                 +QString::number(sheet)
                 );
    emit headParsed(sheet, head);
}

void ExcelWidget::onParseRow(int sheet, int rowNumber, Address a)
{
    emit toDebug(objectName(),
                 "ExcelWidget onParseRow "
                 +QString::number(sheet)+" "
                 +QString::number(rowNumber)
//                 +"\nstr:"+a.getStreet()+"\nb:"+a.getBuild()+"\nk:"+a.getKorp()
                 );
    emit rowParsed(sheet, rowNumber);
}

void ExcelWidget::onFinishParser()
{
//    emit toDebug(objectName(),
//                 "ExcelWidget::onFinishParser()"
//                 );
}

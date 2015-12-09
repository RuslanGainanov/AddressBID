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
    connect(this, SIGNAL(removeEmptySheets(QVector<int>)),
            this, SLOT(onRemoveEmptySheets(QVector<int>)));
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
    foreach (int key, _data.keys()) {
        delete _data.take(key);
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
    connect(_xls, SIGNAL(sheetsAllReaded(QStringList)),
            this, SLOT(onSheetsAllReaded(QStringList)));
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
    for(int i=0; i < head.size(); i++)
        _data.value(sheet)->setHeaderData(i, Qt::Horizontal, head.at(i));

    TableView *view = new TableView(_ui->_tabWidget);
    view->setModel(_data.value(sheet));
    _ui->_tabWidget->addTab(view, _data.value(sheet)->getName());

    _sheetsHead.insert(sheet, head);
    emit headReaded(sheet, head);
}

void ExcelWidget::onReadRow(int sheet, int row, QStringList listRow)
{
//    Q_UNUSED(listRow);
    emit toDebug(objectName(), "ExcelWidget::onReadRow");

    TableModel *tm = _data.value(sheet);
    tm->insertRow(tm->rowCount());
    for(int col=0; col<listRow.size(); col++)
    {
        QModelIndex mi = tm->index(row, col);
        tm->setData(mi,
                    QVariant::fromValue(listRow.at(col)));
    }
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

void ExcelWidget::onSheetsAllReaded(QStringList sheets)
{
    emit toDebug(objectName(),
                 "В документе есть следующий(-ие) лист(-ы):\r\n"
                 +sheets.join(", "));
    for(int i=0; i<sheets.size(); i++)
    {
        _sheets.insert(i, sheets.at(i));
        _data.insert(i, new TableModel(sheets.at(i)));
    }
    emit sheetsAllReaded(sheets);
}

void ExcelWidget::onSheetsReaded(const QMap<int, QString> &sheets)
{
    emit toDebug(objectName(),
                 "В документе есть следующий(-ие) лист(-ы) с информацией:\r\n"
                 +QStringList(sheets.values()).join(", "));
    QMap<int, QString>::const_iterator i = _sheets.constBegin();
    QVector<int> sheetNumbers;
    while (i != _sheets.constEnd()) {
        //если новый список не содержит лист из старого,
        //то этот лист пустой и его нужно удалить
        if(!sheets.contains(i.key()))
            sheetNumbers.push_back(i.key());
        ++i;
    }
//    _sheets=sheets;
    emit sheetsReaded(sheets);
    emit removeEmptySheets(sheetNumbers);
}

void ExcelWidget::onRemoveEmptySheets(QVector<int> numb)
{
    for(int i=0; i<numb.size(); i++)
    {
        _sheets.remove(numb.at(i));
        _sheetsHead.remove(numb.at(i));
        delete _data.take(numb.at(i));
    }
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
    Q_UNUSED(a);
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

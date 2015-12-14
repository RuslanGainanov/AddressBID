#include "parseexcelwidget.h"
#include "ui_parseexcelwidget.h"

ParseExcelWidget::ParseExcelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParseExcelWidget)
{
    ui->setupUi(this);
}

ParseExcelWidget::~ParseExcelWidget()
{
    delete ui;
}

void ParseExcelWidget::onCurrentRowChanged(QString sheet,
                                           int nRow,
                                           MapAEValue data)
{
    _sheet=sheet;
    _nRow=nRow;
    _data=data;
    QString str;
    str=data.value(ENAME, QString());
    ui->lineEditEname->setText(str);
    str=data.value(DISTRICT, QString());
    ui->lineEditDistrict->setText(str);
    str=data.value(STREET, QString());
    ui->lineEditStreet->setText(str);
    str=data.value(BUILD, QString());
    ui->lineEditBuild->setText(str);
    str=data.value(KORP, QString());
    ui->lineEditKorp->setText(str);
    str=data.value(CITY, QString());
    ui->lineEditCity->setText(str);
    str=data.value(FSUBJ, QString());
    ui->lineEditFSubj->setText(str);
    str=data.value(ADDITIONAL, QString());
    ui->lineEditAdditional->setText(str);
    //TODO:
//    str=data.value(LITER, QString());
//    ui->lineEditLiter->setText(str);
}

void ParseExcelWidget::on_lineEditFSubj_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditDistrict_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditCity_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditAdditional_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditEname_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditStreet_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditBuild_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditKorp_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_lineEditLiter_returnPressed()
{
    saveData();
}

void ParseExcelWidget::on_pushButtonSave_clicked()
{
    saveData();
}

void ParseExcelWidget::saveData()
{
    _data[ENAME]=ui->lineEditEname->text();
    _data[DISTRICT]=ui->lineEditDistrict->text();
    _data[STREET]=ui->lineEditStreet->text();
    _data[BUILD]=ui->lineEditBuild->text();
    _data[KORP]=ui->lineEditKorp->text();
    _data[FSUBJ]=ui->lineEditFSubj->text();
    _data[ADDITIONAL]=ui->lineEditAdditional->text();
    _data[CITY]=ui->lineEditCity->text();
//    _data[LITER]=ui->lineEditLiter->text();
    emit dataChanged(_sheet, _nRow, _data);
}

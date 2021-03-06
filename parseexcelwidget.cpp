#include "parseexcelwidget.h"
#include "ui_parseexcelwidget.h"

ParseExcelWidget::ParseExcelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParseExcelWidget)
{
    ui->setupUi(this);
    hideCity2();
}

ParseExcelWidget::~ParseExcelWidget()
{
    delete ui;
}

void ParseExcelWidget::showCity2()
{
    if(_city2Hided)
    {
        ui->pushButtonShowCity2->setVisible(false);
        ui->labelCity2->setVisible(true);
        ui->lineEditCity2->setVisible(true);
        ui->lineEditTypeOfCity2->setVisible(true);
    }
    _city2Hided=false;
}

void ParseExcelWidget::hideCity2()
{
    if(!_city2Hided)
    {
        ui->pushButtonShowCity2->setVisible(true);
        ui->labelCity2->hide();
        ui->lineEditCity2->hide();
        ui->lineEditTypeOfCity2->hide();
    }
    _city2Hided=true;
}

void ParseExcelWidget::onCurrentRowChanged(QString sheet,
                                           int nRow,
                                           MapAEValue data)
{
    _sheet=sheet;
    _nRow=nRow;
    _data=data;
    QString str;
    str=data.value(TYPE_OF_FSUBJ, QString());
//    ui->comboBoxTypeFSubj->setCurrentText(str);
    int index = ui->comboBoxTypeFSubj->findText(str, Qt::MatchExactly);
    if(index != -1)
        ui->comboBoxTypeFSubj->setCurrentIndex(index);
    else
        ui->comboBoxTypeFSubj->setCurrentIndex(0);
    str=data.value(TYPE_OF_STREET, QString());
    ui->lineEditTypeOfStreet->setText(str);
    str=data.value(DISTRICT, QString());
    ui->lineEditDistrict->setText(str);
    str=data.value(STREET, QString());
    ui->lineEditStreet->setText(str);
    str=data.value(BUILD, QString());
    ui->lineEditBuild->setText(str);
    str=data.value(KORP, QString());
    ui->lineEditKorp->setText(str);
    str=data.value(TYPE_OF_CITY1, QString());
    ui->lineEditTypeOfCity->setText(str);
    str=data.value(CITY1, QString());
    ui->lineEditCity->setText(str);
    str=data.value(TYPE_OF_CITY2, QString());
    ui->lineEditTypeOfCity2->setText(str);
    str=data.value(CITY2, QString());
    ui->lineEditCity2->setText(str);
    if(!str.isEmpty())
        showCity2();
    else
        hideCity2();
    str=data.value(FSUBJ, QString());
    ui->lineEditFSubj->setText(str);
    str=data.value(ADDITIONAL, QString());
    ui->lineEditAdditional->setText(str);
    str=data.value(LITERA, QString());
    ui->lineEditLiter->setText(str);
    str=data.value(RAW_ADDR, QString());
    if(str.isEmpty())
        str="Адрес не найден";
    ui->lineEditDatabaseAddr->setText(str);
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

void ParseExcelWidget::on_comboBoxTypeFSubj_activated(int index)
{
    Q_UNUSED(index);
    saveData();
}

void ParseExcelWidget::on_lineEditTypeOfCity_returnPressed()
{
    saveData();
}

void ParseExcelWidget::saveData()
{
    if(ui->comboBoxTypeFSubj->currentText().isEmpty())
        _data[TYPE_OF_FSUBJ]=MapFSubjString[INCORRECT_SUBJ];
    else
        _data[TYPE_OF_FSUBJ]=ui->comboBoxTypeFSubj->currentText();
    _data[TYPE_OF_CITY1]=ui->lineEditTypeOfCity->text();
    _data[TYPE_OF_CITY2]=ui->lineEditTypeOfCity2->text();
    _data[TYPE_OF_STREET]=ui->lineEditTypeOfStreet->text();
    _data[DISTRICT]=ui->lineEditDistrict->text();
    _data[STREET]=ui->lineEditStreet->text();
    _data[BUILD]=ui->lineEditBuild->text();
    _data[KORP]=ui->lineEditKorp->text();
    _data[FSUBJ]=ui->lineEditFSubj->text();
    _data[ADDITIONAL]=ui->lineEditAdditional->text();
    _data[CITY1]=ui->lineEditCity->text();
    _data[CITY2]=ui->lineEditCity2->text();
    _data[LITERA]=ui->lineEditLiter->text();
    emit dataChanged(_sheet, _nRow, _data);
}

void ParseExcelWidget::clearData()
{
    ui->comboBoxTypeFSubj->setCurrentIndex(0);
    ui->lineEditTypeOfCity->clear();
    ui->lineEditTypeOfCity2->clear();
    ui->lineEditTypeOfStreet->clear();
    ui->lineEditDistrict->clear();
    ui->lineEditStreet->clear();
    ui->lineEditBuild->clear();
    ui->lineEditKorp->clear();
    ui->lineEditCity->clear();
    ui->lineEditCity2->clear();
    ui->lineEditFSubj->clear();
    ui->lineEditAdditional->clear();
    ui->lineEditLiter->clear();
    ui->lineEditDatabaseAddr->setText(QString("Адрес не найден"));
}

void ParseExcelWidget::on__pushButtonRemove_clicked()
{
    emit rowRemoved(_sheet, _nRow);
    clearData();
}


void ParseExcelWidget::on_pushButtonShowCity2_clicked()
{
    showCity2();
}

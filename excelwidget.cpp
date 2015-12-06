#include "excelwidget.h"
#include "ui_excelwidget.h"

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::ExcelWidget)
{
    _ui->setupUi(this);
}

ExcelWidget::~ExcelWidget()
{
    delete _ui;
}

void ExcelWidget::on__pushButtonOpen_clicked()
{
    open();
}


void ExcelWidget::open()
{

}

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

#include "debugwidget.h"
#include "ui_debugwidget.h"

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DebugWidget)
{
    _ui->setupUi(this);
}

DebugWidget::~DebugWidget()
{
    delete _ui;
}

void DebugWidget::on__pushButtonClear_clicked()
{
    clear();
}

void DebugWidget::on__pushButtonSave_clicked()
{
    save("");
}


void DebugWidget::add(QString str)
{
    QString s =_ui->_plainTextEdit->toPlainText();
    QString name = sender()->objectName();
    QString time = QDateTime::currentDateTime().toString();
    s.append(time+name+str);
    _ui->_plainTextEdit->setPlainText(s);
}

void DebugWidget::add(int n)
{

}

void DebugWidget::save(QString file)
{

}

void DebugWidget::clear()
{
    _ui->_plainTextEdit->clear();
}

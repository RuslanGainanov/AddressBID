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


void DebugWidget::add(QString objName, QString mes)
{
    if(objName=="ExcelWidget")
        return;
    QString s =_ui->_plainTextEdit->toPlainText();
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    QString append = objName+": "+mes;
    s.append("***\n")
     .append("["+currTime+"]\n"+append+"\n");
    qDebug() << append;
    _ui->_plainTextEdit->setPlainText(s);
}

void DebugWidget::add(QString str)
{
    QString s =_ui->_plainTextEdit->toPlainText();
    QString append = "["+QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz")
            +"]\n"+sender()->objectName()+":"+str;
    s.append("***\n"+append+"\n");
    qDebug() << append;
    _ui->_plainTextEdit->setPlainText(s);
}

void DebugWidget::add(int n)
{
    QString s =_ui->_plainTextEdit->toPlainText();
    QString append = "["+QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz")
            +"] "+sender()->objectName()+":"+QString::number(n)+"\n";
    s.append("\n***\n"+append);
    qDebug() << append;
    _ui->_plainTextEdit->setPlainText(s);
}

void DebugWidget::save(QString file)
{
    Q_UNUSED(file);
}

void DebugWidget::clear()
{
    _ui->_plainTextEdit->clear();
}

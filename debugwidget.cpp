#include "debugwidget.h"
#include "ui_debugwidget.h"

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DebugWidget)
{
    _ui->setupUi(this);
    _logFile.setFileName(LogFileName);
    _logFile.open((QIODevice::OpenModeFlag)LogOpenMode);
    _logStream.setDevice(&_logFile);
    add("***", "***");
}

DebugWidget::~DebugWidget()
{
    delete _ui;
    closeLog();
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
//    if(objName=="ExcelWidget")
//        return;
#ifdef DEBUG_TO_FILE
    qDebug() << "DEBUG:" << objName << ":" << mes;
    addToFile(objName, mes);
#else
    QString s =_ui->_plainTextEdit->toPlainText();
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    QString append = objName+": "+mes;
    s.append("***\n")
     .append("["+currTime+"]\n"+append+"\n");
    qDebug() << append;
    _ui->_plainTextEdit->setPlainText(s);
#endif
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

void DebugWidget::closeLog()
{
    if(_logFile.isOpen())
        _logFile.close();
}

void DebugWidget::addToFile(QString objName, QString mes)
{
    QString currTime=QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    QString s("["+currTime+"]\t["+objName+"]\t["+mes+"]\n");
    _logStream << s;
    _logStream.flush();

}

void DebugWidget::save(QString file)
{
    Q_UNUSED(file);
}

void DebugWidget::clear()
{
    _ui->_plainTextEdit->clear();
}

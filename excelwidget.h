#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include "xlsworker.h"
#include "defines.h"
#include "parser.h"
#include "address.h"

namespace Ui {
class ExcelWidget;
}

class ExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExcelWidget(QWidget *parent = 0);
    ~ExcelWidget();

signals:
    void toDebug(QString);

public slots:
    void open();
    void runThread(QString openFilename);

private slots:
    void on__pushButtonOpen_clicked();

private:
    Ui::ExcelWidget *_ui;
    XlsWorker *_xls;
    QThread *_thread;
    Parser *_parser;
};

#endif // EXCELWIDGET_H

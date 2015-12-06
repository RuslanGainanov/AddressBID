#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include "xlsworker.h"

namespace Ui {
class ExcelWidget;
}

class ExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExcelWidget(QWidget *parent = 0);
    ~ExcelWidget();

public slots:
    void open();

private slots:
    void on__pushButtonOpen_clicked();

private:
    Ui::ExcelWidget *_ui;
};

#endif // EXCELWIDGET_H

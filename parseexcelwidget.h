#ifndef PARSEEXCELWIDGET_H
#define PARSEEXCELWIDGET_H

#include <QWidget>

namespace Ui {
class ParseExcelWidget;
}

class ParseExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParseExcelWidget(QWidget *parent = 0);
    ~ParseExcelWidget();

private:
    Ui::ParseExcelWidget *ui;
};

#endif // PARSEEXCELWIDGET_H

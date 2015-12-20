#ifndef PARSEEXCELWIDGET_H
#define PARSEEXCELWIDGET_H

#include <QWidget>
#include "defines.h"

namespace Ui {
class ParseExcelWidget;
}

class ParseExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParseExcelWidget(QWidget *parent = 0);
    ~ParseExcelWidget();

signals:
    void dataChanged(QString sheet, int nRow, MapAEValue row);

public slots:
    void onCurrentRowChanged(QString sheet, int nRow, MapAEValue data);

private slots:
    void on_lineEditFSubj_returnPressed();
    void on_lineEditDistrict_returnPressed();
    void on_lineEditCity_returnPressed();
    void on_lineEditAdditional_returnPressed();
    void on_lineEditStreet_returnPressed();
    void on_lineEditBuild_returnPressed();
    void on_lineEditKorp_returnPressed();
    void on_lineEditLiter_returnPressed();
    void on_pushButtonSave_clicked();

private slots:
    void saveData();

private:
    Ui::ParseExcelWidget *ui;
    QString _sheet;
    int _nRow;
    MapAEValue _data;
};

#endif // PARSEEXCELWIDGET_H

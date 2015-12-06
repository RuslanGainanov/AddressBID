#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include "database.h"
#include "defines.h"

namespace Ui {
class DatabaseWidget;
}

class DatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatabaseWidget(QWidget *parent = 0);
    ~DatabaseWidget();

public slots:
    void open();
    void clear();
    void viewInfo();

private slots:
    void on__pushButtonOpen_clicked();
    void onReadRow(int row);
    void onReadRows(int rows);
    void onParseRow(int row);
    void onCountRow(int count);
    void onOpenBase();
    void onBaseOpened();

private:
    Ui::DatabaseWidget *_ui;
    Database *_db;

};

#endif // DATABASEWIDGET_H

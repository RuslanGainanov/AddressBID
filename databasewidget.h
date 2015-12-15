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
    Database *getDatabase()
    {
        return _db;
    }

public slots:
    void open();
    void openExisting();
    void clear();
    void viewInfo();

private slots:
    void onReadRow(int row);
    void onReadRows(int rows);
    void onParseRow(int row);
    void onCountRow(int count);
    void onOpenBase();
    void onBaseOpened();

    void on__pushButtonOpen_clicked();
    void on__pushButtonLoadOld_clicked();

signals:
    void toDebug(QString);

private:
    Ui::DatabaseWidget *_ui;
    Database *_db;

    void connectModelWithView(QSqlTableModel *model);

};

#endif // DATABASEWIDGET_H

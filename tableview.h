#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QDebug>

class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    ~TableView();

signals:

public slots:

};

#endif // TABLEVIEW_H

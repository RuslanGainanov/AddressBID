#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>
#include "defines.h"

namespace Ui {
class DebugWidget;
}

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWidget(QWidget *parent = 0);
    ~DebugWidget();

public slots:
    void add(QString str);
    void add(int n);
    void save(QString file);
    void clear();

private slots:
    void on__pushButtonClear_clicked();

    void on__pushButtonSave_clicked();

private:
    Ui::DebugWidget *_ui;
};

#endif // DEBUGWIDGET_H

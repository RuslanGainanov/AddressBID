#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defines.h"
#include "databasewidget.h"
#include "address.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onBaseOpenClicked();

private:
    Ui::MainWindow *_ui;
    DatabaseWidget *_dbw;
};

#endif // MAINWINDOW_H

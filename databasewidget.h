#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QScopedPointer>
#include <QMessageBox>
#include "database.h"
#include "defines.h"
#include "xlsparser.h"

namespace Ui {
class DatabaseWidget;
}

// ********************* class DatabaseWidget ********************
/**
 * \class DatabaseWidget
 * \brief Класс для работы пользователя с БД
 */
class DatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatabaseWidget(QWidget *parent = 0);
    ~DatabaseWidget();
    Database *getDatabase();

public slots:    
    /**
     * \fn bool open();
     * \brief Производит парсинг и открытие БД из csv-файла.
     * \return  статус открытия БД
     */
    bool open();

    /**
     * \fn void openExisting(QString fname);
     * \brief Производит открытие сохраненной и распарсенной БД из файла DefaultBaseName.
     */
    void openExisting();

    /**
     * \fn void openExisting(QString fname);
     * \brief Сигнал отправляемый при изменении модели.
     * \param[in]    count    новое количество строк в модели
     */
    void openExisting(QString fname);

private slots:
    void onBaseOpening();
    void onBaseOpened();
    void onFindButtonClicked();
    void onParseButtonClicked();
    void onClearButtonClicked();
    void onSelectedRows(int count);
    void onProcessOfOpenFinished();
    void onProcessOfParsingFinished();

    void on__pushButtonOpen_clicked();
    void on__pushButtonParseAddr_clicked();
    void on__pushButtonFindParsAddr_clicked();
    void on__pushButtonClear_clicked();
    void on__pushButtonDelete_clicked();


signals:
    void toDebug(QString, QString);
    void exitSignal();
    void selectAddress(Address a);
    void updateModel();

private:
    Ui::DatabaseWidget             *_ui;
    Database                       *_db;
    XlsParser                      *_parser;
    QFutureWatcher<ListAddress>     _futureWatcher;
    QFutureWatcher<void>            _futureWatcherParser;
    QScopedPointer<ListAddress>     _paddr;
    QString                         _csvFileName;
    QThread                        *_thread;

    void readCsvBase(QString openFilename);
};

#endif // DATABASEWIDGET_H

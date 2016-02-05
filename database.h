#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include "address.h"
#include "defines.h"

// ********************* class Database ********************
/**
 * \class Database
 * \brief Класс-обертка для БД
 */
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    ~Database();
    QSqlTableModel *getModel();
    void setBaseName(QString name);
    QString baseName();

    void cancelInsertOperation();
    bool isCanceled();

signals:
    void headReaded(QStringList head);
    void headParsed(MapAddressElementPosition head);
    void rowReaded(int rowNumber);
    void rowParsed(int rowNumber);
    void readedRows(int count);
//    void countRows(int count);
//    void workingWithOpenBase();
    void baseOpened();

    void toDebug(QString,QString);
    void addressFounded(QString sheet, int nRow, Address a);
    void addressNotFounded(QString sheet, int nRow, Address a);

    /**
     * \fn void selectedRows(int count)
     * \brief Сигнал отправляемый при изменении модели.
     * \param[out]    count    новое количество строк в модели
     *
     * Отправляет count==256 когда кол-во строк >= 256.
     */
    void selectedRows(int count);

public slots:
    void openBase(QString filename);
    void removeBase(QString filename);

    void selectAddress(QString sheet, int nRow, Address a);

    void selectAddress(Address a);

    void insertListAddressWithCheck(ListAddress &la);
    void insertAddressWithCheck(Address &a);
    void updateTableModel();
    void clear();

private:
    QSqlTableModel *_model;
    QString _baseName;
    bool _connected;
    bool _canceled;
//    QSet< quint64 > _bids;

    void createModel();
    void removeConnection();
    void createConnection();
    void dropTable();
    void createTable();
};

#endif // DATABASE_H

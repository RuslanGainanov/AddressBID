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
    void baseOpening();
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
    bool removeBase(QString filename);

    void selectAddress(QString sheet, int nRow, Address a);

    void selectAddress(Address a);

    void insertListAddressWithCheck(const ListAddress &la);
    void insertAddressWithCheck(const Address &a);
    void updateTableModel();
    void clear();

private:
    QSqlTableModel *_model;
    QString _baseName;
    bool _connected;
    bool _canceled;
    QSet< quint64 > _bids;
    int _countNotInsertedRows;

    void createModel();
    void removeConnection();
    void createConnection();
    void createTable();
    void dropTable();
};

#endif // DATABASE_H

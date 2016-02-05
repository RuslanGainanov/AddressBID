#ifndef CSVWORKER_H
#define CSVWORKER_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QString>
#include <QStringList>
#include "defines.h"
#include "address.h"

// ********************* class CsvWorker ********************
/**
 * \class CsvWorker
 * \brief Класс для чтения данных БД из csv-файла
 */
class CsvWorker : public QObject
{
    Q_OBJECT
public:
    explicit CsvWorker(QObject *parent = 0);
    ~CsvWorker();
    void setFileName(QString fname);
    void setMaxCountRead(int maxCount);

signals:
    void rowReaded(int rowNumber, QString row);
    void rowReaded(Address a);
    void rowReaded(int rowNumber, QStringList row);
    void rowsReaded(int count);
    void countRows(int count);
    void headReaded(QStringList head);
    void headReaded(QString head);
    void finished();
    void toDebug(QString,QString);

public slots:
//    void process();
//    void process(QString fname, int maxCount=0);

    /**
     * \fn ListAddress readFile(QString fname, int maxCount=0)
     * \brief Функция чтения заданного количества строк из файла
     * \return Cписок объектов класса Address с непарсенными адресами (заполненное поле RAW)
     *
     * Отправка сигналов не происходит
     */
    ListAddress readFile(QString &fname, int maxCount=0);

private:
    QString _fname;
    int _maxCount;

};

#endif // CSVWORKER_H

#ifndef XLSPARSER_H
#define XLSPARSER_H

#include <QObject>
#include "defines.h"
#include "address.h"

// ********************* class XlsParser ********************
/**
 * \class XlsParser
 * \brief Класс, парсинг прочитанной строки Excel-файла
 */
class XlsParser : public QObject
{
    Q_OBJECT
public:
    explicit XlsParser(QObject *parent = 0);
    ~XlsParser();

signals:
    void rowParsed(QString sheet, int row, Address addr);

public slots:
    void onReadHead(const QString sheet,
                    MapAddressElementPosition head);
    void onReadRow(const QString &sheet,
                   const int &rowNumber,
                   const QStringList &row);
    void parseAddress(QString &str, Address &a);

private:
    QMap<QString, MapAddressElementPosition> _mapHead;
    bool parseObject(QString &str, QString &result, QString regPattern, int regCap);
    bool parseObject(QString &str, QStringList &result, QString regPattern);
};

#endif // XLSPARSER_H

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
typedef QList< QStringList > ExcelSheet;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(const QString &name,
                        QObject* pobj = 0);
    ~TableModel();

    QString getName() const;
    QStringList getHeader() const;
    QStringList getRow(int nRow) const;
    ExcelSheet getExcelSheet() const;
    QVariant data(const QModelIndex& index,
                  int                nRole=Qt::DisplayRole) const;
    bool setData(const QModelIndex& index,
                 const QVariant&    value,
                 int                nRole=Qt::EditRole);
    int rowCount(const QModelIndex& = QModelIndex()) const;
    int columnCount(const QModelIndex& = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant & value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &parent);
    bool insertColumns(int position, int columns, const QModelIndex &parent);
    bool removeRows(int position, int rows, const QModelIndex &parent);
    bool removeColumns(int position, int columns, const QModelIndex &parent);


signals:

public slots:

private:
    QString                      m_name;
    ExcelSheet                   m_rowList;
    QStringList                  m_horizontalHeader;


};

#endif // TABLEMODEL_H

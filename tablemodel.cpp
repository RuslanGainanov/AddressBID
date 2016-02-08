#include "tablemodel.h"

TableModel::TableModel(const QString &name, QObject* pobj)
        : QAbstractTableModel(pobj)
        , m_name(name)
{
//    qDebug() << "   TableModel()" << m_name;
}

TableModel::~TableModel()
{
//    qDebug() << "   ~TableModel()" << m_name;
}

QString TableModel::getName() const
{
    return m_name;
}

QStringList TableModel::getHeader() const
{
    return m_horizontalHeader;
}

QStringList TableModel::getRow(int nRow) const
{
    if(nRow>=0 && nRow<m_rowList.size())
        return m_rowList.at(nRow);
    else
        return QStringList();
}

bool TableModel::setRow(const int &nRow, const QStringList &row)
{
    if(row.size()!=columnCount()
            || nRow<0 || nRow>=rowCount())
        return false;
    m_rowList[nRow]=row;
//    for(int i=0; i<row.size(); i++)
//    {
//        m_rowList[nRow][i] = row.at(i);
//        emit dataChanged(index(nRow, i), index(nRow, i));
//    }
    return true;
}

ExcelSheet TableModel::getExcelSheet() const
{
    return m_rowList;
}

QVariant TableModel::data(const QModelIndex& index, int nRole) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if(nRole == Qt::DisplayRole || nRole == Qt::EditRole)
    {
        return m_rowList.at(index.row()).at(index.column());
    }
    else
        return QVariant();
}

bool TableModel::setData(const QModelIndex& index,
             const QVariant&    value,
             int                nRole
            )
{
    if (index.isValid())
    {
        if(nRole == Qt::EditRole)
        {
            m_rowList[index.row()][index.column()] = value.toString();
            emit dataChanged(index, index);
            return true;
        }
    }

//    qDebug() << "setData index not valid" << index.data();
    return false;
}

int TableModel::rowCount(const QModelIndex&) const
{
    return m_rowList.size();
}

QVariant TableModel::headerData(int section,
                                Qt::Orientation orientation,
                                int role
                                ) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    return (orientation == Qt::Horizontal) ? m_horizontalHeader.at(section)
                                           : QString::number(section);
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation,
                   const QVariant & value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    if(orientation == Qt::Horizontal)
        m_horizontalHeader.insert(section, value.toString());
    emit headerDataChanged(orientation, section, section);
    return true;
}

int TableModel::columnCount(const QModelIndex&) const
{
    return m_horizontalHeader.size();
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    int columns = columnCount();
    beginInsertRows(parent, position, position + rows - 1);
    for (int row = 0; row < rows; ++row) {
        QStringList items;
        for (int column = 0; column < columns; ++column)
            items.append("");
        m_rowList.insert(position, items);
    }
    endInsertRows();
    return true;
}

bool TableModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    int rows = rowCount();
    beginInsertColumns(parent, position, position + columns - 1);
    for (int row = 0; row < rows; ++row) {
        for (int column = position; column < columns; ++column) {
            m_rowList[row].insert(position, "");
        }
    }
    endInsertColumns();
    return true;
}

bool TableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position + rows - 1);
    for (int row = 0; row < rows; ++row) {
        m_rowList.removeAt(position);
    }
    endRemoveRows();
    return true;
}
/*!
    Removes a number of columns from the model at the specified position.
    Each row is shortened by the number of columns specified.
*/
bool TableModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    int rows = rowCount();
    beginRemoveColumns(parent, position, position + columns - 1);
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            m_rowList[row].removeAt(position);
            m_horizontalHeader.removeAt(position);
        }
    }
    endRemoveColumns();
    return true;
}

void TableModel::onCurrentRowChanged(QModelIndex curr,QModelIndex prev)
{
    Q_UNUSED(prev);
    emit currentRowChanged(m_name,
                           curr.row(),
                           m_rowList.value(curr.row(), QStringList())
                           );
}

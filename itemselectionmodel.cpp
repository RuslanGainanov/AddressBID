#include "itemselectionmodel.h"

ItemSelectionModel::ItemSelectionModel(QAbstractItemModel * model, QObject * parent):
    QItemSelectionModel(model, parent)
{
//    qDebug() << "ItemSelectionModel constructor";
}

ItemSelectionModel::~ItemSelectionModel()
{
//    qDebug() << "ItemSelectionModel destructor";
}

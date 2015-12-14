#ifndef ITEMSELECTIONMODEL_H
#define ITEMSELECTIONMODEL_H

#include <QObject>
#include <QItemSelectionModel>
#include <QDebug>

class ItemSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    explicit ItemSelectionModel(QAbstractItemModel * model=0, QObject * parent=0);
    ~ItemSelectionModel();
public slots:
signals:
};

#endif // ITEMSELECTIONMODEL_H

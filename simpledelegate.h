#ifndef SIMPLEDELEGATE_H
#define SIMPLEDELEGATE_H

#include <QPainter>
#include <QStyledItemDelegate>
#include "defines.h"

const QString FoundedColor = "#33FF66";         ///< цвет строки для которой найден BID в базе
const QString NotFoundedColor = "#FFA07A";      ///< цвет строки для которой не найден BID в базе
const QString RepeatFoundedColor = "#FFCC33";   ///< цвет строки для которой найдено >1 BID в базе


// ********************* class SimpleDelegate ********************
/**
 * \class SimpleDelegate
 * \brief Вспомогательный класс, выполняющий выделение строк, которые были найдены или не найдены в БД
 */
class SimpleDelegate : public QStyledItemDelegate
{
public:
    SimpleDelegate(QBrush b=QBrush(), QObject* pobj = 0);

    void paint(QPainter*                   pPainter,
               const QStyleOptionViewItem& option,
               const QModelIndex&          index
              ) const;
private:
    QBrush _b;
};

#endif // SIMPLEDELEGATE_H

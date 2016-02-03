#include "simpledelegate.h"

SimpleDelegate::SimpleDelegate(QBrush b, QObject* pobj)
    : QStyledItemDelegate(pobj)
    , _b(b)
{
}

void SimpleDelegate::paint(QPainter*                   pPainter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex&          index
                           ) const
{
    if(!index.isValid())
    {
        QStyledItemDelegate::paint(pPainter, option, index);
        return;
    }
    QRect rect = option.rect;
    pPainter->setBrush(_b);
    pPainter->drawRect(rect);
    QStyledItemDelegate::paint(pPainter, option, index);
}

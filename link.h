#ifndef LINK_H
#define LINK_H

#include <QGraphicsItem>

class Link : public QGraphicsItem
{
public:
    Link(QPair<int, int> start, QPair<int, int> end, QVector<QPair<int, int>> turnPointArray);
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    QVector<QPair<int, int>> pointList;
};

#endif // LINK_H

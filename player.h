#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsItem>
#include "constant.h"

class Player : public QGraphicsItem
{
public:
    enum Direction{
        Up, Down, Left, Right, Stop
    };
    Player(qreal x = 0, qreal y = 0);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setDirection(Direction direction);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void advance(int step) override;
private:
    qreal x;
    qreal y;
    Direction direction;

};

#endif // PLAYER_H

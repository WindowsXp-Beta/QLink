#include "player.h"
#include "constant.h"
#include "box.h"

#include <QPainter>
#include <QDebug>

Player::Player(qreal x, qreal y) : x(x), y(y), direction(Stop)
{

}

QRectF Player::boundingRect() const
{
    return QRectF(0, 0, PLAYER_SIZE, PLAYER_SIZE);
}

QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addEllipse(0, 0, PLAYER_SIZE, PLAYER_SIZE);
    return path;
}

void Player::setDirection(Player::Direction direction)
{
    this->direction = direction;
}

void Player::advance(int step)
{
    if(!step){
        return;
    }
    switch(direction){
    case Up:
        y -= SPEED;
        break;
    case Down:
        y += SPEED;
        break;
    case Left:
        x -= SPEED;
        break;
    case Right:
        x += SPEED;
        break;
    case Stop:
        return;
    }
    setPos(QPointF(x, y));
    QList<QGraphicsItem *> collisions = collidingItems();

    foreach(QGraphicsItem *collidingItem, collisions){
        if(collidingItem->data(BLOCK_TYPE) == Box::Block){
            switch(direction){
            case Up:
                y = collidingItem->y() + BOX_SIZE;
                break;
            case Down:
                y = collidingItem->y() - PLAYER_SIZE;
                break;
            case Left:
                x = collidingItem->x() + BOX_SIZE;
                break;
            case Right:
                x = collidingItem->x() - PLAYER_SIZE;
                break;
        }
    }
    }

    setPos(QPointF(x, y));
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(shape(), Qt::red);

    painter->restore();
}

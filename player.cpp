#include "player.h"
#include "constant.h"
#include "box.h"

#include <QPainter>
#include <QDebug>

const QColor playerColor[2] = {Qt::red, Qt::blue};

Player::Player(int row, int column, int which) :
    which(which),
    direction(Stop),
    chosenBox(nullptr)
{
    corX = -300 + column * BOX_SIZE + BOX_SIZE / 2 - PLAYER_SIZE / 2;
    corY = -300 + row * BOX_SIZE + BOX_SIZE / 2 - PLAYER_SIZE / 2;
    setPos(corX, corY);
}

Player::Player(qreal x, qreal y, int which) :
    corX(x),
    corY(y),
    which(which),
    direction(Stop),
    chosenBox(nullptr)
{
    setPos(corX, corY);
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

void Player::setChosenBox(Box *chosenBox)
{
    this->chosenBox = chosenBox;
}

void Player::advance(int step)
{
    if (!step) {
        return;
    }
    switch (direction) {
    case Up:
        corY -= SPEED;
        break;
    case Down:
        corY += SPEED;
        break;
    case Left:
        corX -= SPEED;
        break;
    case Right:
        corX += SPEED;
        break;
    case Stop:
        return;
    }
    setPos(QPointF(corX, corY));
    QList<QGraphicsItem *> collisions = collidingItems();

    foreach (QGraphicsItem *collidingItem, collisions) {
        QVariant data = collidingItem->data(BLOCK_TYPE);
        if (data == Box::Block) {
            Box *boxItem = static_cast<Box *>(collidingItem);
            if (boxItem->getBorder() == (1 - which)) emit removeOtherChosen(which);
            int posX = boxItem->x();
            int posY = boxItem->y();
            switch (direction) {
            case Up:
                corY = posY + BOX_SIZE;
                break;
            case Down:
                corY = posY - PLAYER_SIZE;
                break;
            case Left:
                corX = posX + BOX_SIZE;
                break;
            case Right:
                corX = posX - PLAYER_SIZE;
                break;
            case Stop:
                break;
            }
            setPos(QPointF(corX, corY));
            boxItem->activate(which);
            if (chosenBox == nullptr || chosenBox == boxItem) chosenBox = boxItem;
            else {
                emit sendBoxCheck(which, boxItem, chosenBox, true);
                chosenBox = nullptr;
                return;
            }
        } else {
            if (data == Box::Shuffle) {
                emit sendShuffle(static_cast<Box *>(collidingItem));
            } else if (data == Box::AddOneSecond) {
                emit sendAddOneSecond(static_cast<Box *>(collidingItem));
            } else if (data == Box::Hint) {
                emit sendHint(static_cast<Box *>(collidingItem));
            }
        }
    }
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(shape(), playerColor[which]);

    painter->restore();
}

#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsObject>

#include "box.h"
#include "constant.h"


class Player : public QGraphicsObject
{
    Q_OBJECT
public:
    enum Direction {
        Up, Down, Left, Right, Stop
    };
    Player(int row, int column, int which);
    Player(qreal x, qreal y, int which);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setDirection(Direction direction);
    void setChosenBox(Box *box);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void sendBoxCheck(int which, Box *box1, Box *box2, bool mode);
    void sendShuffle(Box *shuffleProp);
    void sendHint(Box *hintProp);
    void sendAddOneSecond(Box *addOneSecondProp);
    void removeOtherChosen(int);

protected:
    void advance(int step) override;

private:
    qreal corX;
    qreal corY;
    //which player, can be 1 or 2
    int which;
    Direction direction;
    //The box chosen by this player
    Box *chosenBox;
};

#endif // PLAYER_H

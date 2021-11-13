#ifndef BOX_H
#define BOX_H

#include <QGraphicsItem>

class Box : public QGraphicsItem
{
public:
    enum Type{
        Empty, Block, Shuffle, AddOneSecond, Hint
    };
    Box(int row, int column, Type type, int content);

    void draw();
    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int row;
    int column;
    Type type;
    int content;
};

#endif // BOX_H

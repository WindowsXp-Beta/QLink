#ifndef BOX_H
#define BOX_H

#include <QGraphicsItem>

class Box : public QGraphicsItem
{
public:
    enum Type{
        Block, Shuffle, AddOneSecond, Hint
    };
    Box(int row, int column, Type type, int content, int border = -1, bool isHint = false);
    ~Box();
    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPair<int, int> getPosition();
    void activate(int which);
    void inActivate();
    void chooseHint();
    void unChooseHint();
    bool getHintStatus();
    Type getType();
    int getContent();
    int getBorder();
    void updatePos(int row, int column);

private:
    int row;
    int column;
    Type type;
    int content;
    int border;
    bool isHint;
};

#endif // BOX_H

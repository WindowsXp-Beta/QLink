#include "box.h"
#include "constant.h"

#include <QPainter>
#include <QString>
#include <QDebug>

//0 is player 1, 1 is player 2, 2 is hint
const QColor ACTIVATE_COLOR[3] = {Qt::red, Qt::blue, Qt::gray};

Box::Box(int row, int column, Type type, int content, int border, bool isHint) :
    row(row),
    column(column),
    type(type),
    content(content),
    border(border),
    isHint(isHint)
{
    setData(BLOCK_TYPE, type);
    qreal x = -300 + column * BOX_SIZE;
    qreal y = -300 + row * BOX_SIZE;
    setPos(x, y);
}

Box::~Box()
{
    return ;
//    qDebug() << "delete a box";
}

QRectF Box::boundingRect() const
{
    return QRectF(0, 0, BOX_SIZE, BOX_SIZE);
}

QPainterPath Box::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, BOX_SIZE, BOX_SIZE);
    return path;
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    //draw background
    QString fileName(":/images/assets/");
    if (type == Block) {
        switch(content){
        case 1:
            fileName.append("bucks.jpeg");
            break;
        case 2:
            fileName.append("bulls.jpeg");
            break;
        case 3:
            fileName.append("lakers.jpeg");
            break;
        case 4:
            fileName.append("nets.jpeg");
            break;
        case 5:
            fileName.append("worriers.jpeg");
        }
    } else {
        switch (type) {
        case Shuffle:
            fileName.append("shuffle.jpeg");
            break;
        case AddOneSecond:
            fileName.append("clock.jpeg");
            break;
        case Hint:
            fileName.append("hint.png");
            break;
        }
    }
    QPixmap pix(fileName);
    painter->drawPixmap(0, 0, BOX_SIZE, BOX_SIZE, pix);

    //draw border
    QRectF r = boundingRect();
    if (border > -1 || isHint) {
        if (isHint) {
            painter->setPen(QPen(QBrush(ACTIVATE_COLOR[2]), 4));
        }
        if (border > -1) {
            painter->setPen(QPen(QBrush(ACTIVATE_COLOR[border]), 4));
        }
        int penWidth = painter->pen().width();
        painter->drawRect(QRect(r.x() + penWidth / 2, r.y() + penWidth / 2, r.width() - penWidth, r.height() - penWidth));
    }
    painter->restore();
}

QPair<int, int> Box::getPosition()
{
    return qMakePair(row, column);
}

int Box::getContent()
{
    return content;
}

int Box::getBorder()
{
    return border;
}

void Box::activate(int which)
{
    border = which;
    update();
}

void Box::inActivate()
{
    border = -1;
    update();
}

void Box::chooseHint()
{
    isHint = true;
    update();
}

void Box::unChooseHint()
{
    isHint = false;
    update();
}

bool Box::getHintStatus()
{
    return isHint;
}

Box::Type Box::getType()
{
    return type;
}

void Box::updatePos(int row, int column)
{
    this->row = row;
    this->column = column;
    qreal x = -300 + column * BOX_SIZE;
    qreal y = -300 + row * BOX_SIZE;
    setPos(x, y);
}

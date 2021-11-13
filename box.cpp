#include "box.h"
#include "constant.h"

#include <QPainter>
#include <QString>
#include <QDebug>

Box::Box(int row, int column, Type type, int content):row(row), column(column), type(type), content(content)
{
    setData(BLOCK_TYPE, type);
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
    QString fileName(":/images/");
    switch(content){
    case 1:
        fileName.append("nba_1.jfif");
        break;
    case 2:
        fileName.append("nba_2.jfif");
        break;
    case 3:
        fileName.append("nba_3.jfif");
        break;
    case 4:
        fileName.append("nba_4.jfif");
        break;
    case 5:
        fileName.append("nba_5.jfif");
    }
    QPixmap pix(fileName);
    painter->drawPixmap(0, 0, BOX_SIZE, BOX_SIZE, pix);

    painter->restore();
}

void Box::draw()
{
    qreal x = -300 + column * BOX_SIZE;
    qreal y = -300 + row * BOX_SIZE;
    setPos(x, y);
}

#include "link.h"
#include "constant.h"

#include <QPainter>
#include <QDebug>


Link::Link(QPair<int, int> start, QPair<int, int> end, QVector<QPair<int, int>> turnPointArray)
{
    int rowOffset = start.first;
    int columnOffset = start.second;
    qDebug() << start << end << turnPointArray;
    pointList.append(qMakePair(0, 0));
    foreach (auto pair, turnPointArray) {
        if (pair == qMakePair(-2, -2)) continue;
        pointList.append(qMakePair(pair.first - rowOffset, pair.second - columnOffset));
    }
    pointList.append(qMakePair(end.first - rowOffset, end.second - columnOffset));
    qreal x = -300 + columnOffset * BOX_SIZE + BOX_SIZE / 2;
    qreal y = -300 + rowOffset * BOX_SIZE + BOX_SIZE / 2;
    setPos(x, y);
}


QRectF Link::boundingRect() const
{
    return QRectF(-800, -800, 1600, 1600);
}

void Link::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    QVector<QLineF> lines;
    auto endFlag = pointList.end();
    for (auto it = pointList.begin(); (it + 1) != endFlag; it++) {
        lines.append(QLineF(it->second * BOX_SIZE, it->first * BOX_SIZE, ((it + 1)->second) * BOX_SIZE, ((it + 1)->first) * BOX_SIZE));
    }
    painter->setPen(QPen(QBrush(Qt::green), 10));
    painter->drawLines(lines);
    painter->restore();
}

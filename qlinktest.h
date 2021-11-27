#ifndef QLINKTEST_H
#define QLINKTEST_H

#include "gamewindow.h"

#include <QtTest/QtTest>

class QLinkTest : public QObject
{
    Q_OBJECT
public:
    explicit QLinkTest(QObject *parent = nullptr);

private:
    Box *mockBlock(GameWindow &gameWindow, int row, int column, int content);

private slots:
    void contentMisMatch();
    void blockNotAccessible();
    void blockAccessInOneTurn();
    void blockAccessDirectly();
    void blockAccessTwoTurn();
};

#endif // QLINKTEST_H

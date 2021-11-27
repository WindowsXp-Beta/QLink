#include "qlinktest.h"

QLinkTest::QLinkTest(QObject *parent) : QObject(parent)
{

}

Box *QLinkTest::mockBlock(GameWindow &gameWindow, int row, int column, int content)
{
    Box *mockBox = new Box(row, column, Box::Block, content);
    gameWindow.boxSet.insert(mockBox);
    gameWindow.occupySet.insert(qMakePair(row, column));
    return mockBox;
}

void QLinkTest::contentMisMatch()
{
    GameWindow gameWindow;
    Box *box1 = mockBlock(gameWindow, 0, 0, 0);
    Box *box2 = mockBlock(gameWindow, 0, 1, 1);
    QVERIFY(gameWindow.checkTwoBoxLink(0, box1, box2, false) == false);
}

void QLinkTest::blockNotAccessible()
{
    GameWindow gameWindow;
    Box *box1 = mockBlock(gameWindow, 1, 1, 0);
    Box *box2 = mockBlock(gameWindow, 4, 1, 0);
    mockBlock(gameWindow, 0, 1, 1);
    mockBlock(gameWindow, 1, 2, 1);
    mockBlock(gameWindow, 1, 0, 1);
    mockBlock(gameWindow, 2, 1, 1);
    QVERIFY(gameWindow.checkTwoBoxLink(0, box1, box2, false) == false);
}

void QLinkTest::blockAccessInOneTurn()
{
    GameWindow gameWindow;
    Box *box1 = mockBlock(gameWindow, 0, 0, 0);
    Box *box2 = mockBlock(gameWindow, 2, 2, 0);
    QPair<int, int> turnPair;
    QVERIFY(gameWindow.checkTwoBoxLinkOneTurn(box1->getPosition(), box2->getPosition(), turnPair));
    QVERIFY(turnPair == QPair(2, 0) || turnPair == QPair(0, 2));
}

void QLinkTest::blockAccessDirectly()
{
    GameWindow gameWindow;
    Box *box1 = mockBlock(gameWindow, 0, 0, 0);
    Box *box2 = mockBlock(gameWindow, 0, 1, 0);
    QPair<int, int> turnPair;
    QVERIFY(gameWindow.checkTwoBoxLinkOneTurn(box1->getPosition(), box2->getPosition(), turnPair));
    QVERIFY(turnPair == QPair(-2, -2));
}

void QLinkTest::blockAccessTwoTurn()
{
    GameWindow gameWindow;
    Box *box1 = mockBlock(gameWindow, 0, 0, 0);
    Box *box2 = mockBlock(gameWindow, 2, 2, 0);
    mockBlock(gameWindow, 2, 0, 1);
    mockBlock(gameWindow, 0, 2, 1);
    QVERIFY(gameWindow.checkTwoBoxLink(0, box1, box2, false) == true);
}

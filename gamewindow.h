#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "player.h"
#include "link.h"
#include "box.h"
#include "qlinktest.h"

class QGraphicsScene;
class QGraphicsView;

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

    friend class QLinkTest;

public:
    enum GameStatus {
        Pause, Playing
    };
    enum GameMode {
        Single, Battle
    };
    explicit GameWindow(QWidget *parent = nullptr);
    void startGame(GameMode mode);
    void loadGame();
    ~GameWindow();
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    void createActions();
    void initScene();
    void initSceneBackground();
    void adjustViewSize();

    void gameHelp();
    void sendBackToWindow();
    void handleMoveKeyPress();

    //check if two boxes links, draw the connecting line if connected
    bool checkTwoBoxLink(int which, Box *box1, Box *box2, bool mode = true);
    //check if two boxes can link by one turn
    bool checkTwoBoxLinkOneTurn(QPair<int, int> box1Pos, QPair<int, int> box2Pos, QPair<int, int> &turnPoint);
    void drawConnectionLine(const QPair<int, int> &start, const QPair<int, int> &end, const QVector<QPair<int, int>> &turnPointArray);
    void clearConnectionLine(Link *link);
    void noSolution();

    void movePlayer(int which, Player::Direction direction);
    void generatePlayer(int which);

    void generateProp();
    bool collideWithPlayer(int row, int column);

    void generateMap();
    void drawMap();

    void updateRemainingTime();
    void addTime(Box *addOneSecondItem);
    void removeOtherChosen(int which);

    void shuffleAllItems(Box *shuffleProp);

    void startHint(Box *hintProp);
    void stopHint();
    bool generateHint(bool mode = true);

    void pause();
    void resume();
    void saveGame();
    void closeGame();

    Ui::GameWindow *ui;

    QGraphicsScene *scene;
    QGraphicsView *view;

    GameStatus gameStatus;
    Player *playerList[2];
    int playerScore[2];
    QTimer responseTimer;

    QTimer countDownTimer;
    int gameRemainingTime;

    QTimer hintTimer;
    int hintTimeRemaining;

    QTimer propTimer;

    QSet<int> pressedKeys;
    QSet<Box *> boxSet;
    QSet<QPair<int, int>> occupySet;
    GameMode mode;
    bool isHint;
    QVector<Box *> hintPair;
signals:
    void backToStartWindowSignal();
};

#endif // GAMEWINDOW_H

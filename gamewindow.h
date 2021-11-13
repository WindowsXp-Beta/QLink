#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSet>
#include "player.h"
#include "box.h"

class QGraphicsScene;
class QGraphicsView;

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum GameStatus{
        Pause, Playing
    };
    explicit GameWindow(QWidget *parent = nullptr);
    void startGame();
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
    void movePlayer(int which, Player::Direction direction);

    void generateMap();
    void drawMap();
    void pause();
    void resume();

    Ui::GameWindow *ui;

    QGraphicsScene *scene;
    QGraphicsView *view;
    GameStatus gameStatus;
    Player *player;
    QTimer timer;
    QSet<int> pressedKeys;
    QSet<Box *> boxSet;

signals:
    void backToStartWindowSignal();
};

#endif // GAMEWINDOW_H

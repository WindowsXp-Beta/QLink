#include <QGraphicsView>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
#include <QVector>
#include <random>
#include "constant.h"
#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    scene(new QGraphicsScene(this)),
    view(new QGraphicsView(scene, this)),
    gameStatus(Pause)
{
    ui->setupUi(this);
    setCentralWidget(view);
    createActions();
    initScene();
    initSceneBackground();
}

void GameWindow::startGame()
{
    timer.start(1000 / 33);
    connect(&timer, &QTimer::timeout, this, &GameWindow::handleMoveKeyPress);
    connect(&timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    player = new Player(0, 0);
    scene->addItem(player);
    generateMap();
    drawMap();
}

GameWindow::~GameWindow()
{
    delete player;
    foreach(Box *box, boxSet){
        delete box;
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch (gameStatus) {
    case Pause:{
        gameStatus = Playing;
        resume();
        break;
    }
    case Playing:{
        if(!event->isAutoRepeat()){
            pressedKeys.insert(key);
        }
        break;
    }
    }
}

void GameWindow::pause()
{
    gameStatus = Pause;
    QMessageBox::information(this, tr("pause"), tr("press any key to continue"));
    disconnect(&timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
}

void GameWindow::resume()
{
    gameStatus = Playing;
    connect(&timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
        pressedKeys.remove(event->key());
}

void GameWindow::createActions()
{
    ui->actionhelp->setShortcut(Qt::Key_H);
    connect(ui->actionhelp, &QAction::triggered, this, &GameWindow::gameHelp);
    ui->actionhome->setShortcut(Qt::Key_B);
    connect(ui->actionhome, &QAction::triggered, this, &GameWindow::sendBackToWindow);
    ui->actionpause->setShortcut(Qt::Key_Space);
    connect(ui->actionpause, &QAction::triggered, this, &GameWindow::pause);
}

void GameWindow::sendBackToWindow()
{
    emit backToStartWindowSignal();
}

void GameWindow::handleMoveKeyPress()
{
    if ((pressedKeys.contains(Qt::Key_W) &&
                pressedKeys.contains(Qt::Key_S)) ||
                (pressedKeys.contains(Qt::Key_A) &&
                pressedKeys.contains(Qt::Key_D))) {
        return;
    }
    if(pressedKeys.isEmpty()){
        movePlayer(1, Player::Stop);
        return;
    }
    if (pressedKeys.contains(Qt::Key_W)) {
        movePlayer(1, Player::Up);
    }
    if (pressedKeys.contains(Qt::Key_S)) {
        movePlayer(1, Player::Down);
    }
    if (pressedKeys.contains(Qt::Key_A)) {
        movePlayer(1, Player::Left);
    }
    if (pressedKeys.contains(Qt::Key_D)) {
        movePlayer(1, Player::Right);
    }
}

void GameWindow::movePlayer(int which, Player::Direction direction)
{
    player->setDirection(direction);
}

void GameWindow::generateMap()
{
    QVector<int> boxPos(MAX_ROW * MAX_COLUMN);
    std::iota(boxPos.begin(), boxPos.end(), 0);
    std::shuffle(boxPos.begin(), boxPos.end(), std::default_random_engine());

    for(int i = 0; i < MAX_BOX; i++){
        int pos = boxPos[i];
        int row = pos / MAX_ROW;
        int column = pos % MAX_COLUMN;
        int blockContent = i / BLOCK_NUMBER_EACH_TYPE + 1;
        Box *newBox = new Box(row, column, Box::Block, blockContent);
        scene->addItem(newBox);
        boxSet.insert(newBox);
    }
}

void GameWindow::drawMap()
{
    foreach(Box *box, boxSet){
        box->draw();
    }
}

void GameWindow::initScene()
{
    scene->setSceneRect(-100, -100, 200, 200);
}

void GameWindow::initSceneBackground()
{
//    QPixmap bg(":/images/background.jpg");
//    view->setBackgroundBrush(QBrush(bg));
}

void GameWindow::adjustViewSize()
{
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void GameWindow::gameHelp(){
    QMessageBox::information(this, tr("help"), tr("Welcome to WindowsXp's QLink!"));
}

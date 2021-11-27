#include <QGraphicsView>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
#include <QVector>
#include <random>
#include "constant.h"
#include "gamewindow.h"
#include "ui_gamewindow.h"

//utiliy functions
int randomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max - 1);
    return uni(rng);
}

template <typename T>
void shuffle(QVector<T> &v)
{
    static auto rng = std::default_random_engine {};
    std::shuffle(v.begin(), v.end(), rng);
}

int x2column(qreal x)
{
    return (x + 300) / BOX_SIZE;
}

int y2row(qreal y)
{
    return (y + 300) / BOX_SIZE;
}

void setPlayerRange(QSet<QPair<int, int>> &playerRange, qreal playerX, qreal playerY)
{
    playerRange.insert(qMakePair(y2row(playerY), x2column(playerX)));
    playerRange.insert(qMakePair(y2row(playerY), x2column(playerX + PLAYER_SIZE)));
    playerRange.insert(qMakePair(y2row(playerY + PLAYER_SIZE), x2column(playerX)));
    playerRange.insert(qMakePair(y2row(playerY + PLAYER_SIZE), x2column(playerX + PLAYER_SIZE)));
}


GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    scene(new QGraphicsScene(this)),
    view(new QGraphicsView(scene, this)),
    gameStatus(Pause),
    isHint(false)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::NoFocus);
    setCentralWidget(view);
    createActions();
    ui->countdowntime->setParent(view);
    ui->player1score->setParent(view);
    ui->player2score->setParent(view);
    initScene();
    initSceneBackground();
    hintTimer.setInterval(10000);
    propTimer.setInterval(3000);
    responseTimer.setInterval(1000 / 33);
    countDownTimer.setInterval(1000);
    connect(&responseTimer, &QTimer::timeout, this, &GameWindow::handleMoveKeyPress);
    connect(&responseTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    connect(&hintTimer, &QTimer::timeout, this, &GameWindow::stopHint);
    connect(&propTimer, &QTimer::timeout, this, &GameWindow::generateProp);
    connect(&countDownTimer, &QTimer::timeout, this, &GameWindow::updateRemainingTime);
}

void GameWindow::startGame(GameMode mode)
{
    generateMap();
    generatePlayer(0);
    if (mode == Battle) {
        generatePlayer(1);
    }
    this->mode = mode;

    countDownTimer.start();
    gameRemainingTime = 60;
    ui->countdowntime->setText("Time: " + QString::number(gameRemainingTime));

    hintPair = QVector<Box *>(2, nullptr);

    playerScore[0] = 0;
    playerScore[1] = 0;
    ui->player1score->setText("Player1 Score: " + QString::number(0));
    ui->player2score->setText("Player2 Score: " + QString::number(0));

    responseTimer.start();
    propTimer.start();
}

void GameWindow::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/Users/weixinpeng/Desktop/SEP/QLink/QLink/QLink/archives",
                                                    tr("Text files (*.txt)"));
    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);
    QString line;
    QStringList line2List;
    line = in.readLine();
    line2List = line.split(' ');
    mode = static_cast<GameMode>(line2List[0].toInt());
    gameRemainingTime = line2List[1].toInt();
    isHint = line2List[2].toInt();
    hintPair = QVector<Box *>(2, nullptr);
    if (isHint) {
        line = in.readLine();
        hintTimeRemaining = line2List[0].toInt();
    }
    line = in.readLine();
    line2List = line.split(' ');
    playerScore[0] = line2List[0].toInt();
    Player *player1 = new Player(line2List[1].toDouble(), line2List[2].toDouble(), 0);
    connect(player1, &Player::sendBoxCheck, this, &GameWindow::checkTwoBoxLink);
    connect(player1, &Player::sendShuffle, this, &GameWindow::shuffleAllItems);
    connect(player1, &Player::sendHint, this, &GameWindow::startHint);
    connect(player1, &Player::sendAddOneSecond, this, &GameWindow::addTime);
    connect(player1, &Player::removeOtherChosen, this, &GameWindow::removeOtherChosen);
    playerList[0] = player1;
    scene->addItem(player1);
    if (mode == Battle) {
        line = in.readLine();
        line2List = line.split(' ');
        playerScore[1] = line2List[0].toInt();
        Player *player2 = new Player(line2List[1].toDouble(), line2List[2].toDouble(), 1);
        connect(player2, &Player::sendBoxCheck, this, &GameWindow::checkTwoBoxLink);
        connect(player2, &Player::sendShuffle, this, &GameWindow::shuffleAllItems);
        connect(player2, &Player::sendHint, this, &GameWindow::startHint);
        connect(player2, &Player::sendAddOneSecond, this, &GameWindow::addTime);
        connect(player2, &Player::removeOtherChosen, this, &GameWindow::removeOtherChosen);
        playerList[1] = player2;
        scene->addItem(player2);
    } else {
        playerList[1] = nullptr;
        playerScore[1] = 0;
    }
    //construct box
    while ( !(line = in.readLine()).isNull() ) {
        line2List = line.split(' ');
        int row = line2List[0].toInt();
        int column = line2List[1].toInt();
        Box::Type type = static_cast<Box::Type>(line2List[2].toInt());
        bool boxIsHint = line2List[5].toInt();
        int border = line2List[4].toInt();
        Box *newBox = new Box(row, column, type,
                line2List[3].toInt(),
                border,
                boxIsHint);
        if (isHint && boxIsHint) {
            hintPair.append(newBox);
        }
        if (border == 0 || border == 1) playerList[border]->setChosenBox(newBox);
        boxSet.insert(newBox);
        scene->addItem(newBox);
        if (type == Box::Block) occupySet.insert(qMakePair(row, column));
    }

    ui->countdowntime->setText("Time: " + QString::number(gameRemainingTime));
    ui->player1score->setText("Player1 Score: " + QString::number(playerScore[0]));
    ui->player2score->setText("Player2 Score: " + QString::number(playerScore[1]));

    if (isHint) hintTimer.start(hintTimeRemaining);
    countDownTimer.start();
    responseTimer.start();
    propTimer.start();
}

GameWindow::~GameWindow()
{
    scene->clear();
    delete ui;
}

void GameWindow::startHint(Box *hintProp)
{
    scene->removeItem(hintProp);
    assert(boxSet.remove(hintProp));

    hintTimer.stop();
    hintTimer.start();

    if (!isHint) {
        bool flag = generateHint();
        assert(flag);
    }
    isHint = true;
}

bool GameWindow::generateHint(bool mode)
{
    Box *box1 = nullptr;
    foreach (auto box, boxSet) {
        if (box->data(BLOCK_TYPE) == Box::Block) {
            box1 = box;
            foreach (auto box, boxSet) {
                if (box != box1 && checkTwoBoxLink(-1, box1, box, false)) {
                    if (mode) {
                        hintPair[0] = box1;
                        hintPair[1] = box;
                        box1->chooseHint();
                        box->chooseHint();
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

void GameWindow::stopHint()
{
    hintTimer.stop();
    isHint = false;
    assert(hintPair[0] != nullptr);
    hintPair[0]->unChooseHint();
    hintPair[0] = nullptr;
    assert(hintPair[1] != nullptr);
    hintPair[1]->unChooseHint();
    hintPair[1] = nullptr;
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
        if (!event->isAutoRepeat()) {
            pressedKeys.insert(key);
        }
        break;
    }
    }
}

void GameWindow::addTime(Box *addOneSecondItem)
{
    scene->removeItem(addOneSecondItem);
    assert(boxSet.remove(addOneSecondItem));
    gameRemainingTime += 30;
    ui->countdowntime->setText("Time: " + QString::number(gameRemainingTime));
}

void GameWindow::removeOtherChosen(int which)
{
    qDebug() << "player is " << which;
    playerList[1 - which]->setChosenBox(nullptr);
}

void GameWindow::updateRemainingTime()
{
    gameRemainingTime--;
    if (gameRemainingTime == 0) {
        closeGame();
        return ;
    }
    ui->countdowntime->setText("Time: " + QString::number(gameRemainingTime));
}

void GameWindow::pause()
{
    gameStatus = Pause;
    if (isHint) hintTimeRemaining = hintTimer.remainingTime();
    disconnect(&responseTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    propTimer.stop();
    countDownTimer.stop();
    QMessageBox::information(this, tr("pause"), tr("press any key to continue"));
}

void GameWindow::resume()
{
    gameStatus = Playing;
    propTimer.start();
    countDownTimer.start();
    if (isHint) {
        hintTimer.start(hintTimeRemaining);
    }
    connect(&responseTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
}

void GameWindow::saveGame()
{
    //pause the game
    if (isHint) hintTimeRemaining = hintTimer.remainingTime();
    disconnect(&responseTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    propTimer.stop();
    countDownTimer.stop();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "/Users/weixinpeng/Desktop/SEP/QLink/QLink/QLink/archives/" +
                                                    QString::number(QDateTime::currentSecsSinceEpoch()) +
                                                    ".txt");
    QFile file(fileName);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);
    //save Game metadata
    out << mode << ' ' << gameRemainingTime << ' ' << isHint << '\n';
    if (isHint) out << hintTimer.remainingTime() << '\n';
    //save player
    out << playerScore[0] << ' ' << playerList[0]->x() << ' ' << playerList[0]->y() << '\n';
    if (mode == Battle) out << playerScore[1] << ' ' << playerList[1]->x() << ' ' << playerList[1]->y() << '\n';
    //save all the Box
    foreach (auto box, boxSet) {
        out << box->getPosition().first << ' ' << box->getPosition().second << ' '
            << box->getType() << ' ' << box->getContent() << ' ' << box->getBorder() << ' ' << box->getHintStatus() << '\n';
    }
    closeGame();
}

void GameWindow::noSolution()
{
    countDownTimer.stop();
    hintTimer.stop();
    propTimer.stop();
    responseTimer.stop();
    QMessageBox::warning(this, tr("No solutions"), tr("current map doesn't have solutions, will go back to Home"));
    scene->clear();
    boxSet.clear();
    occupySet.clear();
    pressedKeys.clear();


    emit backToStartWindowSignal();
}

void GameWindow::closeGame()
{
    countDownTimer.stop();
    hintTimer.stop();
    propTimer.stop();
    responseTimer.stop();

    scene->clear();
    occupySet.clear();
    pressedKeys.clear();


    QString message = "Player1's score is " + QString::number(playerScore[0]);
    if (mode == Battle) message += "\nPlayer2's score is " + QString::number(playerScore[1]);
    QMessageBox::information(this, tr("Game over"), message);
    emit backToStartWindowSignal();
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat()) pressedKeys.remove(event->key());
}

void GameWindow::shuffleAllItems(Box *shuffleProp)
{
    scene->removeItem(shuffleProp);
    assert(boxSet.remove(shuffleProp));

    QSet<QPair<int, int>> playerRange;
    setPlayerRange(playerRange, playerList[0]->x(), playerList[0]->y());
    if (mode == Battle) {
        setPlayerRange(playerRange, playerList[1]->x(), playerList[1]->y());
    }
    QVector<int> boxPos(MAX_ROW * MAX_COLUMN);
    std::iota(boxPos.begin(), boxPos.end(), 0);
    shuffle(boxPos);
    occupySet.clear();

    int i = 0, j = boxSet.size(), vectorSize = boxPos.size();
    foreach (auto box, boxSet) {
        int pos = boxPos[i++];
        int row = pos / MAX_COLUMN;
        int column = pos % MAX_COLUMN;
        if (playerRange.contains(qMakePair(row, column))) {
            for (; j < vectorSize; j++) {
                int posTmp = boxPos[j];
                int rowTmp = posTmp / MAX_COLUMN;
                int columnTmp = posTmp % MAX_COLUMN;
                if (!playerRange.contains(qMakePair(rowTmp, columnTmp))) {
                    row = rowTmp;
                    column = columnTmp;
                    break;
                }
            }
        }
        if (box->getType() == Box::Block) occupySet.insert(qMakePair(row, column));
        box->updatePos(row, column);
    }
    if (isHint) {
        hintPair[0]->unChooseHint();
        hintPair[1]->unChooseHint();
        bool flag = generateHint();
        assert(flag);
    }
}

void GameWindow::createActions()
{
    ui->actionhelp->setShortcut(Qt::Key_H);
    connect(ui->actionhelp, &QAction::triggered, this, &GameWindow::gameHelp);
    ui->actionhome->setShortcut(Qt::Key_B);
    connect(ui->actionhome, &QAction::triggered, this, &GameWindow::closeGame);
    ui->actionpause->setShortcut(Qt::Key_Space);
    connect(ui->actionpause, &QAction::triggered, this, &GameWindow::pause);
    ui->actionsave->setShortcut(Qt::Key_Save);
    connect(ui->actionsave, &QAction::triggered, this, &GameWindow::saveGame);
}

void GameWindow::handleMoveKeyPress()
{
    int iterateNumber = mode == Single ? 1 : 2;
    for (int i = 0; i < iterateNumber; i++) {
        int keyUp = i == 0 ? Qt::Key_W : Qt::Key_I;
        int keyDown = i == 0 ? Qt::Key_S : Qt::Key_K;
        int keyLeft = i == 0 ? Qt::Key_A : Qt::Key_J;
        int keyRight = i == 0 ? Qt::Key_D : Qt::Key_L;
        if ((pressedKeys.contains(keyUp) &&
                    pressedKeys.contains(keyDown)) ||
                    (pressedKeys.contains(keyLeft) &&
                    pressedKeys.contains(keyRight))) {
            return;
        }
        if (pressedKeys.isEmpty()) {
            movePlayer(i, Player::Stop);
        }
        if (pressedKeys.contains(keyUp)) {
            movePlayer(i, Player::Up);
        }
        if (pressedKeys.contains(keyDown)) {
            movePlayer(i, Player::Down);
        }
        if (pressedKeys.contains(keyLeft)) {
            movePlayer(i, Player::Left);
        }
        if (pressedKeys.contains(keyRight)) {
            movePlayer(i, Player::Right);
        }
    }
}

bool GameWindow::checkTwoBoxLink(int which, Box *box1, Box *box2, bool mode)
{
    assert(boxSet.contains(box1));
    assert(boxSet.contains(box2));
    auto box1Pos = box1->getPosition();
    auto box2Pos = box2->getPosition();
    auto start = box1Pos;
    auto end = box2Pos;
    auto lowerBox = box1Pos.first < box2Pos.first ? box1Pos : box2Pos;
    auto higherBox = box2Pos.first > box1Pos.first ? box2Pos : box1Pos;
    auto lefterBox = box1Pos.second < box2Pos.second ? box1Pos : box2Pos;
    auto righterBox = box2Pos.second > box1Pos.second ? box2Pos : box1Pos;

    QVector<QPair<int, int>> turnPointArray;
    QPair<int, int> turnPoint;

    if (box1->getContent() != box2->getContent()) goto FAIL;
    if (checkTwoBoxLinkOneTurn(box1Pos, box2Pos, turnPoint)) {
        turnPointArray.append(turnPoint);
        goto FIND;
    }
    //Vertical
    //Up
    for (int i = lowerBox.first + 1; i <= MAX_ROW; i++) {
        auto current = qMakePair(i, lowerBox.second);
        if (occupySet.contains(current)) break;
        if (i == higherBox.first) continue;
        if (checkTwoBoxLinkOneTurn(current, higherBox, turnPoint)) {
            turnPointArray.append(current);
            turnPointArray.append(turnPoint);
            start = lowerBox;
            end = higherBox;
            goto FIND;
        }
    }
    //Down
    for (int i = lowerBox.first - 1; i >= -1; i--) {
        auto current = qMakePair(i, lowerBox.second);
        if (occupySet.contains(current)) break;
        if (checkTwoBoxLinkOneTurn(current, higherBox, turnPoint)) {
            turnPointArray.append(current);
            turnPointArray.append(turnPoint);
            start = lowerBox;
            end = higherBox;
            goto FIND;
        }
    }
    //Horizontal
    //Right
    for (int i = lefterBox.second + 1; i <= MAX_COLUMN; i++) {
        auto current = qMakePair(lefterBox.first, i);
        if (occupySet.contains(current)) break;
        if (i == righterBox.second) continue;
        if (checkTwoBoxLinkOneTurn(current, righterBox, turnPoint)) {
            turnPointArray.append(current);
            turnPointArray.append(turnPoint);
            start = lefterBox;
            end = righterBox;
            goto FIND;
        }
    }
    //Left
    for (int i = lefterBox.second - 1; i >= -1; i--) {
        auto current = qMakePair(lefterBox.first, i);
        if (occupySet.contains(current)) break;
        if (checkTwoBoxLinkOneTurn(current, righterBox, turnPoint)) {
            turnPointArray.append(current);
            turnPointArray.append(turnPoint);
            start = lefterBox;
            end = righterBox;
            goto FIND;
        }
    }
FAIL://No path
    if (mode) {
        box1->inActivate();
        box2->inActivate();
    }
    return false;

FIND:
    if (mode) {
        drawConnectionLine(start, end, turnPointArray);
        assert(boxSet.remove(box1));
        assert(boxSet.remove(box2));
        //must remove boxes first otherwise generateHint will choose same boxes
        if (isHint && (hintPair.contains(box1) || hintPair.contains(box2))) {
            hintPair[0]->unChooseHint();
            hintPair[1]->unChooseHint();
        }
        scene->removeItem(box1);
        scene->removeItem(box2);
        if (!generateHint(isHint)) {
            noSolution();
        }
        occupySet.remove(box1Pos);
        occupySet.remove(box2Pos);
        playerScore[which] += 5;
        switch (which) {
        case 0:
            ui->player1score->setText("Player1 Score: " + QString::number(playerScore[which]));
            break;
        case 1:
            ui->player2score->setText("Player2 Score: " + QString::number(playerScore[which]));
            break;
        }
    }
    return true;
}

void GameWindow::drawConnectionLine(const QPair<int, int> &start, const QPair<int, int> &end, const QVector<QPair<int, int>> &turnPointArray)
{
    Link *newLink = new Link(start, end, turnPointArray);
    scene->addItem(newLink);
    QTimer::singleShot(2000, this, [=](){clearConnectionLine(newLink);});
}

void GameWindow::clearConnectionLine(Link *link)
{
    scene->removeItem(link);
    delete link;
}

bool GameWindow::checkTwoBoxLinkOneTurn(QPair<int, int> box1Pos, QPair<int, int> box2Pos, QPair<int, int> &turnPoint)
{
    bool pathExist[4] = {true, true, true, true};

    auto lowerBox = box1Pos.first < box2Pos.first ? box1Pos : box2Pos;
    auto higherBox = box1Pos.first > box2Pos.first ? box1Pos : box2Pos;
    auto lefterBox = box1Pos.second < box2Pos.second ? box1Pos : box2Pos;
    auto righterBox = box1Pos.second > box2Pos.second ? box1Pos : box2Pos;
    //check link directly
    if (lowerBox.first == higherBox.first) {
        for (int i = lefterBox.second + 1; i < righterBox.second; i++) {
            if (occupySet.contains(qMakePair(lowerBox.first, i))) {
                return false;
            }
        }
        turnPoint = qMakePair(-2, -2);
        return true;
    }
    if (lefterBox.second == righterBox.second) {
        for (int i = lowerBox.first + 1; i < higherBox.first; i++) {
            if (occupySet.contains(qMakePair(i, lefterBox.second))) {
                return false;
            }
        }
        turnPoint = qMakePair(-2, -2);
        return true;
    }
    for (int i = lowerBox.first + 1; i <= higherBox.first; i++) {
        auto current = qMakePair(i, lowerBox.second);
        if (occupySet.contains(current)) {
            pathExist[0] = false;
            break;
        }
    }
    for (int i = lowerBox.first; i < higherBox.first; i++) {
        auto current = qMakePair(i, higherBox.second);
        if (occupySet.contains(current)) {
            pathExist[1] = false;
            break;
        }
    }
    for (int i = lefterBox.second + 1; i <= righterBox.second; i++) {
        auto current = qMakePair(lefterBox.first, i);
        if (occupySet.contains(current)) {
            pathExist[2] = false;
            break;
        }
    }
    for (int i = lefterBox.second; i < righterBox.second; i++) {
        auto current = qMakePair(righterBox.first, i);
        if (occupySet.contains(current)) {
            pathExist[3] = false;
            break;
        }
    }
    //scene 1
    if (lowerBox == lefterBox) {
        if (pathExist[0] && pathExist[3]) {
            turnPoint = qMakePair(higherBox.first, lowerBox.second);
            return true;
        } else if (pathExist[1] && pathExist[2]) {
            turnPoint = qMakePair(lowerBox.first, higherBox.second);
            return true;
        }
    } else { //scene 2
        if (pathExist[0] && pathExist[2]) {
            turnPoint = qMakePair(higherBox.first, lowerBox.second);
            return true;
        } else if (pathExist[1] && pathExist[3]) {
            turnPoint = qMakePair(lowerBox.first, higherBox.second);
            return true;
        }
    }

    return false;
}

void GameWindow::movePlayer(int which, Player::Direction direction)
{
    playerList[which]->setDirection(direction);
}

void GameWindow::generatePlayer(int which)
{
    while (true) {
        int row = randomInt(0, MAX_ROW);
        int column = randomInt(0, MAX_COLUMN);
        if (occupySet.contains(QPair(row, column))) continue;

        //if new player is sounded
        if ((occupySet.contains(QPair(row - 1, column)) || row == 0) &&
                (occupySet.contains(QPair(row + 1, column)) || row == MAX_ROW) &&
                (occupySet.contains(QPair(row, column + 1)) || column == MAX_COLUMN) &&
                (occupySet.contains(QPair(row, column - 1)) || column == 0)) {
            continue;
        }

        Player *newPlayer = new Player(row, column, which);
        playerList[which] = newPlayer;
        connect(newPlayer, &Player::sendBoxCheck, this, &GameWindow::checkTwoBoxLink);
        connect(newPlayer, &Player::sendShuffle, this, &GameWindow::shuffleAllItems);
        connect(newPlayer, &Player::sendHint, this, &GameWindow::startHint);
        connect(newPlayer, &Player::sendAddOneSecond, this, &GameWindow::addTime);
        connect(newPlayer, &Player::removeOtherChosen, this, &GameWindow::removeOtherChosen);
        scene->addItem(newPlayer);
        break;
    }
}

void GameWindow::generateMap()
{
    QVector<int> boxPos(MAX_ROW * MAX_COLUMN);
    std::iota(boxPos.begin(), boxPos.end(), 0);
    shuffle(boxPos);

    for(int i = 0; i < MAX_BOX; i++){
        int pos = boxPos[i];
        int row = pos / MAX_COLUMN;
        int column = pos % MAX_COLUMN;
        int blockContent = i / BLOCK_NUMBER_EACH_TYPE + 1;
        Box *newBox = new Box(row, column, Box::Block, blockContent);
        scene->addItem(newBox);
        boxSet.insert(newBox);
        occupySet.insert(QPair(row, column));
    }
}

void GameWindow::generateProp()
{
    Box::Type propType = static_cast<Box::Type>(randomInt(1, 4));
    int row;
    int column;
    while (true) {
        row = randomInt(0, MAX_ROW);
        column = randomInt(0, MAX_COLUMN);
        if (occupySet.contains(qMakePair(row, column)) || collideWithPlayer(row, column)) continue;
        break;
    }
    Box *newProp = new Box(row, column, propType, 0);
    boxSet.insert(newProp);
    scene->addItem(newProp);
}

bool GameWindow::collideWithPlayer(int row, int column)
{
    QSet<QPair<int, int>> playerRange;
    setPlayerRange(playerRange, playerList[0]->x(), playerList[0]->y());
    if (mode == Battle) {
        setPlayerRange(playerRange, playerList[1]->x(), playerList[1]->y());
    }
    if (playerRange.contains(qMakePair(row, column))) return true;
    return false;
}

void GameWindow::initScene()
{
    scene->setSceneRect(-100, -100, 200, 200);
}

void GameWindow::initSceneBackground()
{
//    QPixmap bg(":/images/assets/background.jpg");
//    view->setBackgroundBrush(QBrush(bg));
}

void GameWindow::adjustViewSize()
{
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void GameWindow::gameHelp(){
    QMessageBox::information(this, tr("help"), tr("Welcome to WindowsXp's QLink!"));
}

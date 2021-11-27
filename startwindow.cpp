#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    connect(&gameWindow, &GameWindow::backToStartWindowSignal, this, &StartWindow::switchToStartWindow);
}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::on_loadgame_clicked()
{
    this->hide();
    gameWindow.loadGame();
    gameWindow.show();
}

void StartWindow::switchToStartWindow()
{
    gameWindow.hide();
    this->show();
}

void StartWindow::on_quitbutton_clicked()
{
    QApplication::quit();
}

void StartWindow::on_battlemode_clicked()
{
    this->hide();
    gameWindow.startGame(GameWindow::Battle);
    gameWindow.show();
}

void StartWindow::on_singlemode_clicked()
{
    this->hide();
    gameWindow.startGame(GameWindow::Single);
    gameWindow.show();
}

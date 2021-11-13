#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    connect(&gameWindow, &GameWindow::backToStartWindowSignal, this, &StartWindow::switchToStartWindow);
    connect(ui->actionload, &QAction::triggered, this, &StartWindow::openFile);
}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::on_loadgame_clicked()
{
    return;
}

void StartWindow::switchToNewGame()
{
    this->hide();
    gameWindow.startGame();
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

void StartWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    qDebug() << fileName;
}

void StartWindow::on_battlemode_clicked()
{
    return;
}

void StartWindow::on_singlemode_clicked()
{
    switchToNewGame();
}

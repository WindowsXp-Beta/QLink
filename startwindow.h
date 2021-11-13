#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "gamewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StartWindow; }
QT_END_NAMESPACE

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private slots:
    void on_loadgame_clicked();
    void switchToStartWindow();

    void on_quitbutton_clicked();
    void openFile();

    void on_battlemode_clicked();

    void on_singlemode_clicked();

private:
    void switchToNewGame();

    Ui::StartWindow *ui;
    GameWindow gameWindow;
};
#endif // STARTWINDOW_H

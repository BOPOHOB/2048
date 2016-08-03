#include "mainwindow.h"
#include "gameboard.h"
#include "scoreboard.h"
#include "settingsboard.h"

MainWindow::MainWindow(QWidget * const parent)
    : QWidget(parent)
    , game(new GameBoard(this))
    , score(new ScoreBoard(this))
    , settings(new SettingsBoard(this))
    , currentBoard(game)
{
    for (QWidget* i : QList<QWidget*>{game, score, settings}) {
        i->move(0, 0);
        i->hide();
    }
    game->show();
    game->setFocus();
    this->setMinimumSize(game->minimumSize());

    QPalette p(this->palette());
    p.setColor(QPalette::Background, 0x00fbf8ef);
    this->setPalette(p);
}

void MainWindow::setCurrentBoard(QWidget * const)
{
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    currentBoard->resize(this->size());
}

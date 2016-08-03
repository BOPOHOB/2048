#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <functional>

class GameBoard;
class ScoreBoard;
class SettingsBoard;

class MainWindow : public QWidget
{
    Q_OBJECT

    void setCurrentBoard(QWidget* const);

protected:
    virtual void resizeEvent(QResizeEvent *);
public:
    explicit MainWindow(QWidget* const parent = 0);

signals:

public slots:

private:
    GameBoard* const game;
    ScoreBoard* const score;
    SettingsBoard* const settings;
    QWidget* currentBoard;
};

#endif // MAINWINDOW_H

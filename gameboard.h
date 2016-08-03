#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QMap>
#include <QRectF>

class QLabel;
class GameWidget;
class QPushButton;
class ScoreLabel;

class GameBoard : public QWidget
{
    Q_OBJECT

    static const int MARGIN = 10;
    static const int ANIMATION_DELAY = 1000;

    bool isStable() const;
    void estimateStableState();

protected:
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *);
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
public:
    explicit GameBoard(QWidget *parent = 0);

private:
    QLabel* const twenty;
    QLabel* const fortyeight;
    ScoreLabel* const best;
    ScoreLabel* const current;
    GameWidget* const game;
    QPushButton* const goSettings;
    QPushButton* const goScore;
    int timerId;

    QPoint mousePressPoint;

    typedef QMap<QWidget*, QRect> WidgetsGeometry;
    WidgetsGeometry stableState;
};

#endif // GAMEBOARD_H

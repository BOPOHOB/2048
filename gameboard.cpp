#include "gameboard.h"

#include <QLabel>
#include <QPushButton>
#include <QTimerEvent>
#include <QTextStream>
#include <QKeyEvent>
#include <QMouseEvent>

#include "gamewidget/gamewidget.h"
#include "application.h"
#include "scorelabel.h"
#include "scorelabel.h"

GameBoard::GameBoard(QWidget *parent)
    : QWidget(parent)
    , twenty(new QLabel("20", this))
    , fortyeight(new QLabel("48", this))
    , best(new ScoreLabel("РЕКОРД", 0x00cd3301, this))
    , current(new ScoreLabel("СЧЕТ", 0x00776e65, this))
    , game(new GameWidget(this))
    , goSettings(new QPushButton(this))
    , goScore(new QPushButton(this))
    , timerId(-1)
{
    this->setMinimumWidth(best->width() + current->width() + 3 * MARGIN);
    this->setMinimumHeight(300);
    game->setGame(Application::getGame());
    connect(Application::me(), &Application::gameChanged, game, &GameWidget::setGame);

    estimateStableState();
    const WidgetsGeometry::const_iterator end(stableState.end());
    for (WidgetsGeometry::const_iterator it(stableState.begin()); it != end; ++it) {
        it.key()->setGeometry(it.value());
    }
    for (QLabel* w : QList<QLabel*> { twenty, fortyeight }) {
        QFont font;
        font.setPixelSize(80);
        w->setFont(font);
        w->setFixedSize(w->sizeHint());
    }
    connect(Application::me(), &Application::scoreChanged, current, &ScoreLabel::setScore);
    connect(Application::me(), &Application::bestScoreChanged, best, &ScoreLabel::setScore);
    current->setScore(Application::getGame()->game().score());
    best->setScore(Application::bestScore());
}

void GameBoard::resizeEvent(QResizeEvent *)
{
    estimateStableState();
}

void GameBoard::estimateStableState()
{
    const int vStray(twenty->height() + goSettings->height() + best->height() + MARGIN * 5);
    const int hStray(best->width() * 2 + MARGIN * 4);
    const QSize presizeV(this->width() - MARGIN * 2,
                         this->height() - vStray);
    const QSize presizeH(this->width() - hStray,
                         this->height() - MARGIN * 2);
    const QSize& gameSize(game->game().size());
    //raw tablet edge in vertical case
    const int tabletEdgeV(std::min(presizeV.width() / gameSize.width(), presizeV.height() / gameSize.height()));
    //raw tablet edge in orisontal case
    const int tabletEdgeH(std::min(presizeH.width() / gameSize.width(), presizeH.height() / gameSize.height()));
    //select most usefully case
    const Qt::Orientation orientation(tabletEdgeV > tabletEdgeH ? Qt::Vertical : Qt::Horizontal);
    const int edge(orientation == Qt::Vertical ? std::min(presizeV.width(), presizeV.height()) : std::min(presizeH.width(), presizeH.height()));
    const QSize gameWidgetSize(edge, edge);
    const QSize mainSize(
        orientation == Qt::Vertical ? gameWidgetSize.width() + MARGIN * 2 : gameWidgetSize.width() + hStray,
        orientation == Qt::Vertical ? gameWidgetSize.height() + vStray : gameWidgetSize.height() + MARGIN * 2);
    const QPoint topLeft((this->width() - mainSize.width()) / 2,
                         (this->height() - mainSize.height()) / 2);
    const int vyMargin((this->height() - twenty->height() - gameWidgetSize.height() - best->height() - goScore->height()) / 5);
    Q_ASSERT(vyMargin >= MARGIN || orientation != Qt::Vertical);
    const int hyMargin(MARGIN);
    const int yMargin(orientation == Qt::Vertical ? vyMargin : hyMargin);
    const int gameY(orientation == Qt::Vertical ? yMargin * 3 + twenty->height() + best->height() : topLeft.y() + yMargin);
    stableState[game] = QRect(QPoint(topLeft.x() + (orientation == Qt::Vertical ? MARGIN : MARGIN * 2 + best->width()),
                                     gameY),
                              gameWidgetSize);
    const int twentyX(orientation == Qt::Vertical || gameY > twenty->height() + MARGIN ?
                          this->width() / 2 - twenty->width() :
                          topLeft.x() + MARGIN + (best->width() - twenty->width()) / 2);
    stableState[twenty] = QRect(QPoint(twentyX, yMargin), twenty->size());
    const int fortyeightX(orientation == Qt::Vertical ||
                          gameY > twenty->height() + MARGIN ? this->width() / 2 :
                                                              topLeft.x() + mainSize.width() - (current->width() + fortyeight->width()) / 2 - MARGIN);
    stableState[fortyeight] = QRect(QPoint(fortyeightX, yMargin), fortyeight->size());
    stableState[best] = QRect(QPoint(orientation == Qt::Vertical ? (this->width() - MARGIN) / 2 - best->width() : topLeft.x() + MARGIN,
                                     orientation == Qt::Vertical ? yMargin * 2 + twenty->height() : (this->height() - best->height()) / 2),
                              best->size());
    stableState[current] = QRect(QPoint(orientation == Qt::Vertical ? (this->width() + MARGIN) / 2 : topLeft.x() + mainSize.width() - current->width() - MARGIN,
                                        orientation == Qt::Vertical ? yMargin * 2 + twenty->height() : (this->height() - best->height()) / 2),
                                 best->size());
    stableState[goScore] = QRect(QPoint(topLeft.x() + (orientation == Qt::Vertical ? MARGIN : MARGIN + (best->width() - goScore->width()) / 2),
                                        (orientation == Qt::Vertical ? this->height() : topLeft.y() + mainSize.height()) - yMargin - goScore->height()),
                                 goScore->size());
    stableState[goSettings] = QRect(QPoint(topLeft.x() + (mainSize.width() - (orientation == Qt::Vertical ? MARGIN + goScore->width() :
                                                                                                            MARGIN + (best->width() + goScore->width()) / 2)),
                                    (orientation == Qt::Vertical ? this->height() : topLeft.y() + mainSize.height()) - yMargin - goScore->height()),
                                    goSettings->size());
    if (timerId < 0) {
        timerId = this->startTimer(1000 / 80);
    }
}

void GameBoard::timerEvent(QTimerEvent * e)
{
    if (e->timerId() != timerId) {
        e->ignore();
        qDebug() << "unknown timer" << e->timerId() << timerId;
    }
    e->accept();
    if (isStable()) {
        this->killTimer(timerId);
        timerId = -1;
        return;
    }
    for (WidgetsGeometry::const_iterator i(stableState.begin()); i != stableState.end(); ++i) {
        static const int LIM(5);
        QRect now(i.key()->geometry());
        const QRect& stableRect(i.value());
        const QPoint dA(stableRect.topLeft() - now.topLeft());
        const QPoint dB(stableRect.bottomRight() - now.bottomRight());
        if (dA.manhattanLength() < LIM && dB.manhattanLength() < LIM) {
            now = stableRect;
        } else {
            now.setTopLeft(now.topLeft() + dA * .5);
            now.setBottomRight(now.bottomRight() + dB * .5);
        }
        i.key()->setGeometry(now);
    }
}

void GameBoard::keyPressEvent(QKeyEvent * e)
{
    switch (e->key()) {
    case Qt::Key_Up:
        Application::me()->move(Qt::TopEdge);
        e->accept();
        break;
    case Qt::Key_Down:
        Application::me()->move(Qt::BottomEdge);
        e->accept();
        break;
    case Qt::Key_Left:
        Application::me()->move(Qt::LeftEdge);
        e->accept();
        break;
    case Qt::Key_Right:
        Application::me()->move(Qt::RightEdge);
        e->accept();
        break;
    default:
        e->ignore();
    }
}

bool GameBoard::isStable() const
{
    const WidgetsGeometry::const_iterator end(stableState.end());
    for (WidgetsGeometry::const_iterator it(stableState.begin()); it != end; ++it) {
        if (it.key()->geometry() != it.value()) {
            return false;
        }
    }
    return true;
}

void GameBoard::mousePressEvent(QMouseEvent * e)
{
    mousePressPoint = e->pos();
}

void GameBoard::mouseReleaseEvent(QMouseEvent * e)
{
    const QPoint p(e->pos() - mousePressPoint);
    const int d(std::max(abs(p.x()), abs(p.y())));
    if (d * 6 < std::min(this->width(), this->height())) {
        return;
    }
    if (abs(p.x()) < abs(p.y())) {
        if (p.y() > 0) {
            Application::me()->move(Qt::BottomEdge);
        } else {
            Application::me()->move(Qt::TopEdge);
        }
    } else {
        if (p.x() > 0) {
            Application::me()->move(Qt::RightEdge);
        } else {
            Application::me()->move(Qt::LeftEdge);
        }
    }
}

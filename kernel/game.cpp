#include "game.h"
#include <random>
#include <QTime>
#include <QDebug>

bool operator<(const QPoint& a, const QPoint& b)
{
    return a.x() == b.x() ? a.y() < b.y() : a.x() < b.x();
}

uint qHash(const QPoint& p)
{
    return qHash(p.x(),p.y());
}

Game::Game(const QSize& s)
    : s(s)
    , score(0)
{
    addNewTablet();
    addNewTablet();
}

Game Game::move(const Qt::Edge side) const
{
    Game r(*this);
    const QPoint direction(([&side]()->QPoint {
        switch (side) {
        case Qt::BottomEdge:
            return QPoint( 0,  1);
        case Qt::LeftEdge:
            return QPoint(-1,  0);
        case Qt::RightEdge:
            return QPoint( 1,  0);
        case Qt::TopEdge:
            return QPoint( 0, -1);
        }
    })());
    QPoint cur(([&side, &r]()->QPoint {
        switch (side) {
        case Qt::BottomEdge:
            return QPoint(0, r.s.height() - 1);
        case Qt::RightEdge:
            return QPoint(r.s.width() - 1, 0);
        case Qt::LeftEdge:
        case Qt::TopEdge:
            return QPoint(0, 0);
        }
    })());
    const bool isVertical(side & (Qt::TopEdge | Qt::BottomEdge));
    const int steps(isVertical ? r.s.height() : r.s.width());
    const int width(isVertical ? r.s.width() : r.s.height());
    const QPoint step(isVertical, !isVertical);
    QSet<QPoint> locked;
    bool moved(false);
    for (int i(0); i != steps; ++i) {
        for (int j(0); j != width; ++j) {
            const TabletMap::iterator itm(r.tablets.find(cur));
            if (itm != r.tablets.end()) {
                int counter(0);
                TabletMap::iterator neighbour(r.tablets.end());
                while (counter < i && (neighbour = r.tablets.find(itm.key() + direction * (counter + 1))) == r.tablets.end()) {
                    ++counter;
                }
                if (neighbour != r.tablets.end() && neighbour.value() == itm.value() && !locked.contains(neighbour.key())) {
                    neighbour.value() <<= 1;
                    r.score += neighbour.value();
                    locked.insert(neighbour.key());
                    r.tablets.erase(itm);
                    moved = true;
                } else if (counter) {
                    moved = true;
                    const int value(itm.value());
                    const QPoint pos(itm.key() + direction * counter);
                    r.tablets.erase(itm);
                    r.tablets[pos] = value;
                }
            }
            cur += step;
        }
        cur -= direction + step * width;
    }
    r.addNewTablet();
    return moved ? r : *this;
}

bool Game::addNewTablet()
{
    QList<TabletMap::key_type> availablePositions;
    if (s.isEmpty()) {
        return false;
    }
    for (int i(0); i != s.width(); ++i) {
        for (int j(0); j != s.height(); ++j) if (!tablets.contains(TabletMap::key_type(i, j))) {
            availablePositions << TabletMap::key_type(i, j);
        }
    }
    if (availablePositions.empty()) {
        return false;
    }
    static std::default_random_engine generator(QTime::currentTime().msec());
    std::uniform_real_distribution<double> likeJS(0.0,1.0);
    const int value(likeJS(generator) < 0.9 ? 2 : 4);
    std::uniform_int_distribution<> randomItem(0,availablePositions.size() - 1);
    tablets.insert(availablePositions.at(randomItem(generator)), value);
    return true;
}

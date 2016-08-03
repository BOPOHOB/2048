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
    , count(0)
{
    addNewTablet();
    addNewTablet();
}

Game::Game()
    : s(3,4)
    , count(0)
{
    addNewTablet();
    addNewTablet();
}

bool Game::isGameOver() const
{
    if (s.width() * s.height() != tablets.size()) {
        return false;
    }
    for (int i(0); i != s.width(); ++i) {
        for (int j(0); j != s.height(); ++j) {
            const TabletMap::const_iterator here(tablets.find(QPoint(i,j)));
            Q_ASSERT(here != tablets.end());
            auto test = [&here, this](const QPoint& p) {
                if (p.x() < s.width() && p.y() < s.height()) {
                    const TabletMap::const_iterator there(tablets.find(p));
                    Q_ASSERT(there != tablets.end());
                    if (there.value() == here.value()) {
                        return false;
                    }
                }
                return true;
            };
            if (!test(QPoint(i + 1, j)) || !test(QPoint(i, j + 1))) {
                    return false;
            }
        }
    }
    return true;
}

QMap<int, int> Game::move(const Qt::Edge side)
{
    const QPoint direction(([&side]()->QPoint {
        switch (side) {
        case Qt::BottomEdge:
            return QPoint( 0,  1);
        case Qt::LeftEdge:
            return QPoint(-1,  0);
        case Qt::RightEdge:
            return QPoint( 1,  0);
        default:
        case Qt::TopEdge:
            return QPoint( 0, -1);
        }
    })());
    QPoint cur(([&side, this]()->QPoint {
        switch (side) {
        case Qt::BottomEdge:
            return QPoint(0, s.height() - 1);
        case Qt::RightEdge:
            return QPoint(s.width() - 1, 0);
        default:
        case Qt::LeftEdge:
        case Qt::TopEdge:
            return QPoint(0, 0);
        }
    })());
    const bool isVertical(side & (Qt::TopEdge | Qt::BottomEdge));
    const int steps(isVertical ? s.height() : s.width());
    const int width(isVertical ? s.width() : s.height());
    const QPoint step(isVertical, !isVertical);
    QSet<QPoint> locked;
    bool moved(false);
    QMap<int, int> mergeList;
    for (int i(0); i != steps; ++i) {
        for (int j(0); j != width; ++j) {
            const TabletMap::iterator itm(tablets.find(cur));
            if (itm != tablets.end()) {
                int counter(0);
                TabletMap::iterator neighbour(tablets.end());
                while (counter < i && (neighbour = tablets.find(itm.key() + direction * (counter + 1))) == tablets.end()) {
                    ++counter;
                }
                if (neighbour != tablets.end() && neighbour.value() == itm.value() && !locked.contains(neighbour.key())) {
                    neighbour.value() <<= 1;
                    mergeList.insert(itm.value().id(), neighbour.value().id());
                    count += neighbour.value();
                    locked.insert(neighbour.key());
                    tablets.erase(itm);
                    moved = true;
                } else if (counter) {
                    const Tablet value(itm.value());
                    const QPoint pos(itm.key() + direction * counter);
                    tablets.erase(itm);
                    tablets.insert(pos, value);
                    moved = true;
                }
            }
            cur += step;
        }
        cur -= direction + step * width;
    }
    if (moved) {
        addNewTablet();
    }
    return mergeList;
}

void Game::addNewTablet()
{
    QList<TabletMap::key_type> availablePositions;
    if (s.isEmpty() || tablets.size() == s.width() * s.height()) {
        return;
    }
    for (int i(0); i != s.width(); ++i) {
        for (int j(0); j != s.height(); ++j) if (!tablets.contains(TabletMap::key_type(i, j))) {
            availablePositions << TabletMap::key_type(i, j);
        }
    }
    Q_ASSERT(!availablePositions.empty());
    static std::default_random_engine generator(QTime::currentTime().msec());
    std::uniform_real_distribution<double> likeJS(0.0,1.0);
    const Tablet value(likeJS(generator) < 0.9 ? 2 : 4);
    std::uniform_int_distribution<> randomItem(0,availablePositions.size() - 1);
    tablets.insert(availablePositions.at(randomItem(generator)), value);
}

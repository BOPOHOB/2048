#ifndef GAME_H
#define GAME_H

#include <QSize>
#include <QMap>
#include <QPoint>
#include "tablet.h"

bool operator<(const QPoint&, const QPoint&);
uint qHash(const QPoint&);

class Game
{
    void addNewTablet();
public:
    typedef QMap<QPoint, Tablet> TabletMap;

    Game(const QSize& s);
    Game();

    const QSize& size() const { return s; }
    const TabletMap& tabletMap() const { return tablets; }

    QMap<int, int> move(const Qt::Edge);
    quint64 score() const { return count; }
    bool isGameOver() const;
private:
    QSize s;
    quint64 count;
    TabletMap tablets;
};

#endif // GAME_H

#ifndef GAME_H
#define GAME_H

#include <QSize>
#include <QMap>
#include <QPoint>

bool operator<(const QPoint&, const QPoint&);
uint qHash(const QPoint&);

class Game
{
public:
    typedef QMap<QPoint, int> TabletMap;

    Game(const QSize& s);

    const QSize& size() const { return s; }
    const TabletMap& tabletMap() const { return tablets; }

    Game move(const Qt::Edge) const;
    bool addNewTablet();
private:
    QSize s;
    int score;
    TabletMap tablets;
};

#endif // GAME_H

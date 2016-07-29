#ifndef STATE_H
#define STATE_H

#include "kernel/game.h"

class State
{
public:
    State(const QSize&);

    void move(const Qt::Edge);
    const QSize& size() const { return now.size(); }
    const Game::TabletMap& tabletMap() const { return now.tabletMap(); }

private:
    Game now;
};

#endif // STATE_H

#include "widgetstate.h"
#include <QTimerEvent>
#include <QVector2D>
#include <QDebug>

WidgetState::WidgetState(QObject * const parent)
    : QObject(parent)
{
    this->startTimer(1000 / 80);

}

void WidgetState::setGame(const GameObject* const v)
{
    now.clear();
    tail = v;
}

bool WidgetState::isMoving()
{
    if (tail->game().tabletMap().size() != now.size()) {
        return true;
    }
    for (StateBuf::ConstIterator i(now.begin()), end(now.end()); i != end; ++i) {
        const Game::TabletMap::const_iterator j(tail->game().tabletMap().find(QPoint(i.value().position.x() + 0.5, i.value().position.y() + 0.5)));
        if (j == tail->game().tabletMap().end()) {
            return true;
        }
        if (QPointF(j.key().x(), j.key().y()) != i.value().position) {
            return true;
        }
        if (j.value().id() != i.key()) {
            return true;
        }
        if (i.value().scale <= 0.99 || i.value().scale >= 1.01) {
            return true;
        }
        if (i.value().value != j.value()) {
            return true;
        }
    }
    return false;
}

QPointF WidgetState::step(const QPointF& curPos, const QPoint& tailPos)
{
    return step(curPos, QPointF(tailPos.x(), tailPos.y()));
}

QPointF WidgetState::step(const QPointF& curPos, const QPointF& tailPos)
{
    QPointF d(tailPos.x() - curPos.x(), tailPos.y() - curPos.y());
    const qreal dest(sqrt(d.x() * d.x() + d.y() * d.y()));
    const qreal speed(0.2 * (tail->size().width() + tail->size().height()) / 8);
    if (dest < speed) {
        return d;
    } else {
        return d / (dest / speed);
    }
}

const FrameState WidgetState::currentFrame() const
{
    FrameState r(now.values());
    std::sort(r.begin(), r.end(), [](const ItemState& a, const ItemState& b) { return a.value < b.value; });
    return r;
}

void WidgetState::timerEvent(QTimerEvent * e)
{
    e->accept();
    if (!isMoving()) {
        return;
    }
    for (Game::TabletMap::const_iterator it(tail->game().tabletMap().begin()), end(tail->game().tabletMap().end()); it != end; ++it) {
        const int id(it.value().id());
        StateBuf::iterator tabletState(now.find(id));
        if (tabletState != now.end()) {
            QPointF& curPos(tabletState.value().position);
            if (curPos != it.key()) {
                curPos += step(curPos, it.key());
            }
        } else {
            tabletState = now.insert(id, ItemState(QPointF(it.key().x(), it.key().y()), it.value()));
        }
    }
    for (ItemState& tablet : now) {
        if (tablet.scale <= 0.999) {
            tablet.scale += 0.1;
        } else if (tablet.scale >= 1.001) {
            tablet.scale -= 0.1;
        } else {
            tablet.scale = (tablet.scale - 1.0) * 0.1 + 1.0;
        }
    }
    merging = merging.unite(tail->takeCapturedMerge());
    MergeMap::iterator it(merging.begin());
    while (it != merging.end()) {
        const StateBuf::iterator eater(now.find(it.value()));
        StateBuf::iterator eat(now.find(it.key()));
        Q_ASSERT(eater != now.end());
        //It's should be ossure when the item appear end merged betwen atimation frames.
        //As a result item wasn't come to now map and directly pass to
        //merge map. No reasons to paint it. just remove.
        if (eat == now.end()) {
            const Game::TabletMap& tablets(tail->game().tabletMap());
            Game::TabletMap::const_iterator jt(tablets.begin());
            while (jt != tablets.end() && jt.value().id() != eater.key()) {
                ++jt;
            }
            Q_ASSERT(jt != tablets.end());
            eater.value().position = jt.key();
            eater.value().value = jt.value();
            it = merging.erase(it);
        } else {
            //move eat under eater if eater is eat also to remove after thet
            if (merging.contains(it.value())) {
                eat.value().position = eater.value().position;
            } else {
                eat.value().position += step(eat.value().position, eater.value().position);
            }
            if (eat.value().position == eater.value().position) {
                eater.value().scale = 1.2;
                for (const Tablet& i: tail->game().tabletMap()) if (i.id() == eater.key()) {
                    eater.value().value = i;
                }
                now.erase(eat);
                it = merging.erase(it);
            } else {
                ++it;
            }
        }
    }
    emit newFrameReady();
}

#include "widgetstate.h"
#include <QTimerEvent>
#include <QVector2D>
#include <QDebug>

WidgetState::WidgetState(QObject * const parent)
    : QObject(parent)
{
    this->startTimer(1000 / 80);
}

void WidgetState::move(const Qt::Edge e)
{
    merging = merging.unite(tail.move(e));
#ifndef QT_NO_DEBUG
    Q_ASSERT(merging.keys() == merging.uniqueKeys());
#endif
}

bool WidgetState::isMoving()
{
    if (tail.tabletMap().size() != now.size()) {
        qDebug() << tail.tabletMap().size() << now.size();
        return true;
    }
    for (StateBuf::ConstIterator i(now.begin()), end(now.end()); i != end; ++i) {
        const Game::TabletMap::const_iterator j(tail.tabletMap().find(QPoint(i.value().position.x() + 0.5, i.value().position.y() + 0.5)));
        if (j == tail.tabletMap().end()) {
            qDebug() << now
                      << tail.tabletMap().keys();
            return true;
        }
        if (QPointF(j.key().x(), j.key().y()) != i.value().position) {
            return true;
        }
        if (j.value().id() != i.key()) {
            qDebug() << "keyvalue" << j.value().id() << i.key();
            return true;
        }
        if (i.value().scale <= 0.99 || i.value().scale >= 1.01) {
            qDebug() << i.value().scale;
            return true;
        }
        if (i.value().value != j.value()) {
            qDebug() << i.value().value << j.value();
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
    const qreal speed(0.5 * (tail.size().width() + tail.size().height()) / 8);
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
    static int flajochek(0);
    qDebug() << "loop" << flajochek++;
    for (Game::TabletMap::const_iterator it(tail.tabletMap().begin()), end(tail.tabletMap().end()); it != end; ++it) {
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
    MergeMap::iterator it(merging.begin());
    qDebug() << "merging" << merging;
    while (it != merging.end()) {
        qDebug() << "merge" << it.key() << "<<" << it.value();
        const StateBuf::iterator eater(now.find(it.value()));
        const StateBuf::iterator eat(now.find(it.key()));
        Q_ASSERT(eater != now.end());
        Q_ASSERT(eat != now.end());
        qDebug() << eat.value().position, eater.value().position;
        if (merging.contains(it.value())) {
            eat.value().position = eater.value().position;
        } else {
            eat.value().position += step(eat.value().position, eater.value().position);
        }
        qDebug() << eat.value().position;
        if (eat.value().position == eater.value().position) {
            eater.value().scale = 1.2;
            eater.value().value <<= 1;
            now.erase(eat);
            it = merging.erase(it);
        } else {
            ++it;
        }
    }
    qDebug() << "emit newFrameReady";
    emit newFrameReady();
}

#ifndef WIDGETSTATE_H
#define WIDGETSTATE_H

#include <QObject>
#include <QDebug>
#include "kernel/game.h"

struct ItemState {
    QPointF position;
    qreal scale;
    quint64 value;

    ItemState(const QPointF& p, const quint64& v) : position(p), scale(0.0), value(v) {}
};

inline QDebug operator<<(QDebug o, const ItemState& i)
{
    return o << "ItemState(" << i.position << i.scale << i.value << ")";
}

class QTimer;

typedef QList<ItemState> FrameState;

class WidgetState : public QObject
{
    Q_OBJECT

    void timerEvent(QTimerEvent *);

    bool isMoving();

    QPointF step(const QPointF &curPos, const QPoint& tailPos);
    QPointF step(const QPointF &curPos, const QPointF& tailPos);
    typedef QMap<int, ItemState> StateBuf;
public:
    WidgetState(QObject * const parent);

    void move(const Qt::Edge);
    const QSize& size() const { return tail.size(); }
    const FrameState currentFrame() const;
    void setGame(const Game& v) { tail = v; }
    const Game& game() const { return tail; }

signals:
    void newFrameReady();

private:
    QMap<int, ItemState> now;
    typedef QMap<int, int> MergeMap;
    MergeMap merging;
    Game tail;

};

#endif // WIDGETSTATE_H

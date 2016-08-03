#ifndef WIDGETSTATE_H
#define WIDGETSTATE_H

#include <QObject>
#include <QDebug>
#include "kernel/gameobject.h"

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

    const QSize& size() const { return tail->size(); }
    const FrameState currentFrame() const;
    void setGame(const GameObject* const v);
    const Game& game() const { return tail->game(); }

signals:
    void newFrameReady();

private:
    QMap<int, ItemState> now;
    typedef QMap<int, int> MergeMap;
    MergeMap merging;
    const GameObject* tail;

};

#endif // WIDGETSTATE_H

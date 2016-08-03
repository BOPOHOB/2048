#include "gameobject.h"

GameObject::GameObject(const QSize &size, QObject *parent)
    : QObject(parent)
    , g(size)
{

}


void GameObject::move(const Qt::Edge e)
{
    const int score(g.score());
    capturedMerge = capturedMerge.unite(g.move(e));
#ifndef QT_NO_DEBUG
    Q_ASSERT(capturedMerge.keys() == capturedMerge.uniqueKeys());
#endif

    if (score != g.score()) {
        emit scoreChanged(g.score());
    }
    if (g.isGameOver()) {
        emit gameOver();
    }
}

QMap<int, int> GameObject::takeCapturedMerge() const
{
    const QMap<int, int> r(capturedMerge);
    capturedMerge.clear();
    return r;
}

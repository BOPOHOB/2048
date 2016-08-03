#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include "game.h"

class GameObject : public QObject
{
    Q_OBJECT
public:
    GameObject(const QSize& size, QObject* parent = 0);

    const QSize& size() const { return g.size(); }

    void move(const Qt::Edge);
    const Game& game() const { return g; }
    QMap<int, int> takeCapturedMerge() const;
signals:
    void scoreChanged(quint64);
    void gameOver();

private:
    Game g;
    mutable QMap<int, int> capturedMerge;
};

#endif // GAMEOBJECT_H

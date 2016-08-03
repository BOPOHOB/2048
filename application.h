#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "kernel/gameobject.h"

bool operator<(const QSize&, const QSize&);

class Application : public QApplication
{
    Q_OBJECT

    typedef QMap<QSize, GameObject*> Games;
    typedef QMap<QSize, quint32> Score;
public:
    Application(int argc, char** argv);
    static GameObject* getGame();
    static Application* me() { return static_cast<Application*>(qApp); }
    void setCurrentSize(const QSize&);
    void move(Qt::Edge);
    static quint64 bestScore();
private:
    Score best;
    Games games;
    QSize currentSize;

signals:
    void scoreChanged(quint64);
    void bestScoreChanged(quint64);
    void gameOver();
    void gameChanged(const GameObject*);
};

#endif // APPLICATION_H

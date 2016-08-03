#include "application.h"
#include <QFont>

bool operator<(const QSize& a, const QSize& b)
{
    return a.width() == b.width() ? a.height() < b.height() : a.width() < b.width();
}

Application::Application(int argc, char **argv)
    : QApplication(argc, argv)
    , currentSize(4,4)
{
    setFont(QFont("\"Clear Sans\", \"Helvetica Neue\", Arial, sans-serif", -1, 600));
}

GameObject* Application::getGame()
{
    Games& games(static_cast<Application*>(qApp)->games);
    QSize& currentSize(static_cast<Application*>(qApp)->currentSize);
    if (games.contains(currentSize)) {
        return games[currentSize];
    } else {
        GameObject* const game(new GameObject(currentSize, qApp));
        if (!me()->best.contains(currentSize)) {
            me()->best.insert(currentSize, 0);
        }
        connect(game, &GameObject::scoreChanged, [](quint64 v){
            emit me()->scoreChanged(v);
            if (v > me()->best[me()->currentSize]) {
                me()->best[me()->currentSize] = v;
                emit me()->bestScoreChanged(v);
            }
        });
        connect(game, &GameObject::gameOver, [](){
            emit me()->gameOver();
        });
        return games.insert(currentSize, game).value();
    }
}

void Application::setCurrentSize(const QSize& s)
{
    if (s == static_cast<Application*>(qApp)->currentSize) {
        return;
    }
    static_cast<Application*>(qApp)->currentSize = s;
    emit gameChanged(getGame());
    emit scoreChanged(getGame()->game().score());
    emit bestScoreChanged(bestScore());
}

void Application::move(Qt::Edge e)
{
    me()->getGame()->move(e);
}

quint64 Application::bestScore()
{
    if (me()->best.contains(me()->currentSize)) {
        me()->best.insert(me()->currentSize, 0);
    }
    return me()->best[me()->currentSize];
}

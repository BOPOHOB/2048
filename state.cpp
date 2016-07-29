#include "state.h"

State::State(const QSize & s)
    : now(s)
{

}

void State::move(const Qt::Edge e)
{
    now = now.move(e);
}

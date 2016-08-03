#include "scorelabel.h"
#include <QLabel>
#include <QStyle>
#include <QDebug>
#include <QRegion>

ScoreLabel::ScoreLabel(const QString& text, const QColor& bg, QWidget* const parent)
    : QWidget(parent)
    , lbl(new QLabel(text, this))
    , score(new QLabel(this))
{
    this->setFixedSize(280, 120);
    QFont f;
    f.setPixelSize(28);
    for (QLabel* l : QList<QLabel*>{lbl, score}) {
        l->setFont(f);
        l->setAlignment(Qt::AlignCenter);
        l->setFixedWidth(this->width());

    }
    auto setColor([](QWidget* w, QPalette::ColorRole role, const QColor& color){
        QPalette p(w->palette());
        p.setColor(role, color);
        w->setPalette(p);
    });
    setColor(lbl, QPalette::WindowText, 0x00d5d0bd);
    setColor(score, QPalette::WindowText, 0x00FFEFFF);
    lbl->move(0,20);
    score->move(0,78);

    setColor(this, QPalette::Background, bg);
    this->setAutoFillBackground(true);

    //http://stackoverflow.com/questions/24612576/how-to-create-smooth-rounded-corners-on-a-popup-widget-in-qt
    const int radius(2);
    QRegion verticalRegion(0, radius, width(), height() - 2 * radius);
    QRegion horizontalRegion(radius, 0, width() - 2 * radius, height());
    QRegion circle(0, 0, 2 * radius, 2 * radius, QRegion::Ellipse);

    QRegion region = verticalRegion.united(horizontalRegion);
    region = region.united(circle);
    region = region.united(circle.translated(width() - 2 * radius, 0));
    region = region.united(circle.translated(width() - 2 * radius, height() - 2 * radius));
    region = region.united(circle.translated(0, height() - 2 * radius));

    setMask(region);

}

void ScoreLabel::setScore(const quint64 v)
{
    score->setText(QString::number(v));
}

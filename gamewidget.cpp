#include "gamewidget.h"
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QStaticText>
#include <QKeyEvent>

const QPainterPath GameWidget::TABLET_PATH = ([]()->QPainterPath{
    QPainterPath r;
    r.addRoundedRect(0, 0, GameWidget::TABLET_EDGE, GameWidget::TABLET_EDGE, GameWidget::TABLET_RADIUS, GameWidget::TABLET_RADIUS);
    return r;
})();
const QString GameWidget::FONT_FAMILY = "\"Clear Sans\", \"Helvetica Neue\", Arial, sans-serif";

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , animation(new QTimer(this))
    , current(QSize(30, 20))
{
    animation->setInterval(1000 / 30);
}

GameWidget::~GameWidget()
{

}

void GameWidget::setState(const State &v)
{
    current = v;
    this->repaint();
}

void GameWidget::initPainter(QPainter* painter)
{
    QWidget::initPainter(painter);
    const QSize fs(fieldSize());
    const qreal k(std::min(this->width() * 1.0 / fs.width(),
                           this->height() * 1.0 / fs.height()));
    const QPointF offset((this->width() - fs.width() * k) / 2.0,
                        (this->height() - fs.height() * k) / 2.0);
    painter->translate(offset);
    painter->scale(k, k);
    QBrush brush(painter->brush());
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
}

void GameWidget::resizeEvent(QResizeEvent*)
{
    backgroundRepaint();
}

QSize GameWidget::fieldSize() const
{
    const QSize& field(current.size());
    return QSize(STEP * field.width() + MARGIN, STEP * field.height() + MARGIN);
}

void GameWidget::backgroundRepaint()
{
    QPainter painter(&bg);
    initPainter(&painter);
    QBrush brush(painter.brush());
    brush.setColor(colors.bg);
    QPainterPath bgPath;
    bgPath.addRoundedRect(QRectF(QPoint(0, 0), fieldSize()), BG_RADIUS, BG_RADIUS);
    painter.fillPath(bgPath, brush);
    painter.translate(MARGIN, MARGIN);
    brush.setColor(colors.field);
    for (int i(0); i != current.size().width(); ++i) {
        painter.save();
        for (int j(0); j != current.size().height(); ++j) {
            painter.fillPath(TABLET_PATH, brush);
            painter.translate(0, STEP);
        }
        painter.restore();
        painter.translate(STEP, 0);
    }
}

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPicture(0, 0, bg);
    initPainter(&painter);
    painter.translate(MARGIN, MARGIN);
    QBrush brush(painter.brush());
    QPen pen(painter.pen());
    QFont font(FONT_FAMILY, -1, 600);
    for (Game::TabletMap::const_iterator it(current.tabletMap().begin()); it != current.tabletMap().end(); ++it) {
        painter.save();

        painter.translate(STEP * it.key().x(), STEP * it.key().y());
        const QMap<int, QColor>::const_iterator tabletColor(colors.tablet.find(it.value()));
        if (tabletColor != colors.tablet.end()) {
            brush.setColor(tabletColor.value());
        } else if (colors.tablet.empty()) {
            brush.setColor(0x00cd3301);
        } else {
            brush.setColor(colors.tablet.last());
        }
        const QMap<int, QColor>::const_iterator textColor(colors.font.find(it.value()));
        if (textColor != colors.font.end()) {
            pen.setColor(textColor.value());
        } else if (colors.font.empty()) {
            pen.setColor(0x00f4faf0);
        } else {
            pen.setColor(colors.font.last());
        }
        painter.fillPath(TABLET_PATH, brush);
        painter.setPen(pen);

        if (it.value() < 100) {
            font.setPixelSize(55);
        } else if (it.value() < 1000) {
            font.setPixelSize(45);
        } else if (it.value() < 10000) {
            font.setPixelSize(35);
        } else if (it.value() < 100000) {
            font.setPixelSize(25);
        } else {
            font.setPixelSize(20);
        }
        QFontMetricsF metric(font);
        painter.setFont(font);
        painter.drawText(QRect(MARGIN, TABLET_EDGE / 2.0 - metric.height() / 2.0,
                               TABLET_EDGE - (MARGIN << 1), TABLET_EDGE - (MARGIN << 1)),Qt::AlignHCenter, QString::number(it.value()));

        painter.restore();
    }
}

GameWidget::Colors::Colors()
    : bg(0x00bbada0)
    , field(0x00cdc1b4)
{
    tablet[1 <<  1] = 0x00eee4da;
    tablet[1 <<  2] = 0x00ece0c8;
    tablet[1 <<  3] = 0x00f2b179;
    tablet[1 <<  4] = 0x00f59563;
    tablet[1 <<  5] = 0x00f57c5f;
    tablet[1 <<  6] = 0x00f35e3d;
    tablet[1 <<  7] = 0x00edce71;
    tablet[1 <<  8] = 0x00e3c853;
    tablet[1 <<  9] = 0x00f0be39;
    tablet[1 << 10] = 0x00ffaf00;
    tablet[1 << 11] = 0x00ff8b00;
    tablet[1 << 12] = 0x00f47518;
    tablet[1 << 13] = 0x00cd3301;
    font[1 <<  1] = 0x0048413b;
    font[1 <<  2] = 0x0048413b;
    font[1 <<  3] = 0x00f4faf0;
    font[1 <<  4] = 0x00f4faf0;
    font[1 <<  5] = 0x00f4faf0;
    font[1 <<  6] = 0x00f4faf0;
    font[1 <<  7] = 0x00f4faf0;
    font[1 <<  8] = 0x00f4faf0;
    font[1 <<  9] = 0x00f4faf0;
    font[1 << 10] = 0x00f4faf0;
    font[1 << 11] = 0x00f4faf0;
    font[1 << 12] = 0x00f4faf0;
    font[1 << 13] = 0x00f4faf0;
}

void GameWidget::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Left) {
        current.move(Qt::LeftEdge);
        this->repaint();
    } else if (e->key() == Qt::Key_Right) {
        current.move(Qt::RightEdge);
        this->repaint();
    } else if (e->key() == Qt::Key_Up) {
        current.move(Qt::TopEdge);
        this->repaint();
    } else if (e->key() == Qt::Key_Down) {
        current.move(Qt::BottomEdge);
        this->repaint();
    }
}

#include "gamewidget.h"
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QStaticText>
#include <QKeyEvent>

const QPainterPath GameWidget::TABLET_PATH = ([]()->QPainterPath{
    static const int TABLET_EDGE = 106;
    static const int TABLET_RADIUS = 2;
    QPainterPath r;
    r.addRoundedRect(-TABLET_EDGE / 2.0, -TABLET_EDGE / 2.0,
                     TABLET_EDGE, TABLET_EDGE,
                     TABLET_RADIUS, TABLET_RADIUS);
    return r;
})();

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , current(new WidgetState(this))
{
    connect(current, SIGNAL(newFrameReady()), this, SLOT(repaint()));
}

GameWidget::~GameWidget()
{

}

void GameWidget::setGame(const GameObject * const v)
{
    current->setGame(v);
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
    repaintBackground();
}

QSize GameWidget::fieldSize() const
{
    if (!current) {
        return QSize();
    }
    const QSize& field(current->size());
    return QSize(STEP * field.width() + MARGIN, STEP * field.height() + MARGIN);
}

void GameWidget::repaintBackground()
{
    if (!current) {
        bg = QPicture();
        return;
    }
    QPainter painter(&bg);
    initPainter(&painter);
    QBrush brush(painter.brush());
    brush.setColor(colors.background());
    QPainterPath bgPath;
    bgPath.addRoundedRect(QRectF(QPoint(0, 0), fieldSize()), BG_RADIUS, BG_RADIUS);
    painter.fillPath(bgPath, brush);
    painter.translate(MARGIN + TABLET_EDGE / 2.0, MARGIN + TABLET_EDGE / 2.0);
    brush.setColor(colors.tabletBackgroung());
    for (int i(0); i != current->size().width(); ++i) {
        painter.save();
        for (int j(0); j != current->size().height(); ++j) {
            painter.fillPath(TABLET_PATH, brush);
            painter.translate(0, STEP);
        }
        painter.restore();
        painter.translate(STEP, 0);
    }
}

void GameWidget::paintEvent(QPaintEvent* e)
{
    if (!current) {
        QWidget::paintEvent(e);
        return;
    }
    QPainter painter(this);
    painter.drawPicture(0, 0, bg);
    initPainter(&painter);
    painter.translate(MARGIN + TABLET_EDGE / 2.0, MARGIN + TABLET_EDGE / 2.0);
    QBrush brush(painter.brush());
    QPen pen(painter.pen());
    QFont font;
    for (const ItemState& tablet : current->currentFrame()) {
        painter.save();
        painter.translate(STEP * tablet.position.x(), STEP * tablet.position.y());
        painter.scale(tablet.scale, tablet.scale);

        pen.setColor(colors.tabletColorForValue(tablet.value));
        brush.setColor(colors.fontColorForValue(tablet.value));
        painter.setPen(pen);
        painter.fillPath(TABLET_PATH, brush);

        font.setPixelSize(fontPixelSize(tablet.value));
        QFontMetricsF metric(font);
        painter.setFont(font);
        painter.drawText(QRectF(MARGIN - TABLET_EDGE / 2.0, -metric.height() / 2.0,
                                TABLET_EDGE - (MARGIN << 1), TABLET_EDGE - (MARGIN << 1)),
                         Qt::AlignHCenter, QString::number(tablet.value));

        painter.restore();
    }
}

int GameWidget::fontPixelSize(const quint64& v)
{
    if (v < 100) {
        return 55;
    } else if (v < 1000) {
        return 45;
    } else if (v < 10000) {
        return 35;
    } else if (v < 100000) {
        return 25;
    } else {
        return 20;
    }
}

QSize GameWidget::sizeHint() const
{
    const QSize& gameSize(game().size());
    return QSize((TABLET_EDGE + MARGIN) * gameSize.width() + MARGIN, (TABLET_EDGE * MARGIN) * gameSize.height() + MARGIN);
}

const QColor& GameWidget::Colors::fontColorForValue(quint64 value) const
{
    const int id(log2f(value) - 0.5f);
    if (id < tablet.size()) {
        return tablet.at(id);
    } else if (tablet.empty()) {
        static const QColor c(0x00cd3301);
        return c;
    } else {
        return tablet.last();
    }
}

const QColor& GameWidget::Colors::tabletColorForValue(quint64 value) const
{
    const int id(log2f(value) - 0.5f);
    if (id < font.size()) {
        return font.at(id);
    } else if (font.empty()) {
        static const QColor c(0x00f4faf0);
        return c;
    } else {
        return font.last();
    }
}

GameWidget::Colors::Colors()
    : bg(0x00bbada0)
    , field(0x00cdc1b4)
    , tablet(13)
    , font(13)
{
    tablet[ 0] = 0x00eee4da;
    tablet[ 1] = 0x00ece0c8;
    tablet[ 2] = 0x00f2b179;
    tablet[ 3] = 0x00f59563;
    tablet[ 4] = 0x00f57c5f;
    tablet[ 5] = 0x00f35e3d;
    tablet[ 6] = 0x00edce71;
    tablet[ 7] = 0x00e3c853;
    tablet[ 8] = 0x00f0be39;
    tablet[ 9] = 0x00ffaf00;
    tablet[10] = 0x00ff8b00;
    tablet[11] = 0x00f47518;
    tablet[12] = 0x00cd3301;
    font[ 0] = 0x0048413b;
    font[ 1] = 0x0048413b;
    font[ 2] = 0x00f4faf0;
    font[ 3] = 0x00f4faf0;
    font[ 4] = 0x00f4faf0;
    font[ 5] = 0x00f4faf0;
    font[ 6] = 0x00f4faf0;
    font[ 7] = 0x00f4faf0;
    font[ 8] = 0x00f4faf0;
    font[ 9] = 0x00f4faf0;
    font[10] = 0x00f4faf0;
    font[11] = 0x00f4faf0;
    font[12] = 0x00f4faf0;
}

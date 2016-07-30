#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QPicture>
#include "widgetstate.h"

class QTimer;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    static const int EDGE = 500;
    static const int MARGIN = 15;
    static const int TABLET_EDGE = 106;
    static const int TABLET_RADIUS = 2;
    static const int BG_RADIUS = 4;
    static const int STEP = MARGIN + TABLET_EDGE;
    static const QPainterPath TABLET_PATH;
    static const QString FONT_FAMILY;

    GameWidget(QWidget *parent = 0);
    ~GameWidget();

    void setGame(Game& v);

    class Colors
    {
    public:
        const QColor& fontColorForValue(quint64 value) const;
        const QColor& tabletColorForValue(quint64 value) const;
        const QColor& background() const { return bg; }
        const QColor& tabletBackgroung() const { return field; }

        Colors();

    private:
        QColor bg;
        QColor field;
        QVector<QColor> tablet;
        QVector<QColor> font;
    };

    void setColors(const Colors& c) { colors = c; }
    const Colors& getColors() const { return colors; }

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent*);
    virtual void initPainter(QPainter* painter);
    virtual void keyPressEvent(QKeyEvent*);

private:
    QSize fieldSize() const;
    void repaintBackground();
    static int fontPixelSize(const quint64&);

    WidgetState* const current;
    Colors colors;
    QPicture bg;

signals:
    void arrowPressed(Qt::Edge);
};

#endif // GAMEWIDGET_H

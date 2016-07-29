#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QPicture>
#include "state.h"

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

    void setState(const State&v);

    struct Colors
    {
        QColor bg;
        QColor field;
        QMap<int, QColor> tablet;
        QMap<int, QColor> font;

        Colors();
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
    void backgroundRepaint();

    State current;
    QTimer* const animation;
    Colors colors;
    QPicture bg;
};

#endif // GAMEWIDGET_H

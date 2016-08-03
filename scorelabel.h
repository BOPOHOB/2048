#ifndef SCORELABEL_H
#define SCORELABEL_H

#include <QWidget>
class QLabel;

class ScoreLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ScoreLabel(const QString& label, const QColor& bg, QWidget * const parent = 0);

    void setScore(const quint64);
private:
    QLabel* const lbl;
    QLabel* const score;
};

#endif // SCORELABEL_H

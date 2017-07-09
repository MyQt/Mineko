#ifndef SONGSLIDER_H
#define SONGSLIDER_H
#include <QSlider>
#include <QMouseEvent>

class SongSlider: public QSlider
{
    Q_OBJECT
public:
    SongSlider(QWidget *parent = 0);
    void setSliderStyleSheet(QColor color, bool bHorizontal);
protected:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // SONGSLIDER_H

#ifndef VOLPANNEL_H
#define VOLPANNEL_H
#include <QSlider>
#include <QMouseEvent>

class volPannel : public QSlider
{
    Q_OBJECT
public:
    volPannel(QWidget *parent = 0);
    void setSliderStyleSheet(QColor color, bool bHorizontal);
protected:
    void mousePressEvent(QMouseEvent *ev);
private:

    void leaveEvent(QEvent *);
};

#endif // VOLPANNEL_H

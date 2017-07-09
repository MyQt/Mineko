#include "songslider.h"

SongSlider::SongSlider(QWidget *parent):QSlider(parent)
{

}

void SongSlider::mousePressEvent(QMouseEvent *ev)
{
    double pos = 0.0;
    if (orientation() == Qt::Horizontal)
    {
        pos = (ev->pos().x() / (double)width());
    } else
    {
        pos = 1.0-(ev->pos().y() / (double)height());
    }
    setValue(pos*(maximum()-minimum()) + minimum());
    emit sliderPressed();
}

void SongSlider::setSliderStyleSheet(QColor color, bool bHorizontal)
{
    QString styleSheet;
    if (bHorizontal)
    {
        // 设置主体
        styleSheet.append("QSlider{border:15px solid transparent; border-style:outset;}");
        // 槽
        styleSheet.append("QSlider::groove:horizontal {border: 2px solid;\
                            height: 10px;margin: 0px 0;left: 5px; right: 5px;}");
        // 设置中间的那个滑动的键
//        styleSheet.append("QSlider::handle:horizontal {border: -20px ;width: 20px; height: 20px;\
//                          margin:-5px 6px -5px 6px;\
//                          border-radius:20px;\
//                          background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5;");
//        styleSheet.append(",stop:0 ").append(arcColor.name());
//        styleSheet.append(",stop:1 ").append(arcColor.name()).append(");}");
        // 还没有滑上去的地方
        styleSheet.append("QSlider::add-page:horizontal{\
                          background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 ").append(color.name());
        styleSheet.append(",stop:0.25 ").append(color.darker(250).name());
        styleSheet.append(",stop:0.5 ").append(color.darker(300).name());
        styleSheet.append(",stop:1 ").append(color.name()).append(");}");
        // 已经划过的地方
        styleSheet.append("QSlider::sub-page:horizontal{\
                          background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 ").append(color.name());
        styleSheet.append(",stop:0.25 ").append(color.lighter(50).name());
        styleSheet.append(",stop:0.5 ").append(color.lighter(100).name());
        styleSheet.append(",stop:1 ").append(color.name()).append(");}");

    } else
    {
        // 设置主体
        styleSheet.append("QSlider{border:5px solid transparent;}");
        styleSheet.append("QSlider::groove:vertical {border: 2px solid transparent;\
                            width: 5px;border-radius: 1px;\
        padding-left:-1px; padding-right:-1px; padding-top:-1px;  padding-bottom:-1px;}");
        // 设置中间的那个滑动的键
//        styleSheet.append("QSlider::handle:vertical {border: 0px ;width: 15px;\
//                          margin: -5px -5px -5px -5px;}");
        // 还没有滑上去的地方
        styleSheet.append("QSlider::add-page:vertical{\
                          background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 ").append(color.name());
        styleSheet.append(",stop:0.25 ").append(color.darker(250).name());
        styleSheet.append(",stop:0.5 ").append(color.darker(300).name());
        styleSheet.append(",stop:1 ").append(color.name()).append(");}");
        // 已经划过的地方
        styleSheet.append("QSlider::sub-page:vertical{\
                          background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 ").append(color.name());
        styleSheet.append(",stop:0.25 ").append(color.lighter(50).name());
        styleSheet.append(",stop:0.5 ").append(color.lighter(100).name());
        styleSheet.append(",stop:1 ").append(color.name()).append(");}");

    }
    setStyleSheet(styleSheet);
}

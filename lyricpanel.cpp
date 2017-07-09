#include "lyricpanel.h"

LyricPanel::LyricPanel(QWidget *parent):QLabel(parent)
{

}

LyricPanel::~LyricPanel()
{

}

void LyricPanel::mousePressEvent(QMouseEvent *ev)
{
    emit mousePressSignal(ev);
}

void LyricPanel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    emit mouseDoubleClickSignal(ev);
}

void LyricPanel::mouseMoveEvent(QMouseEvent *ev)
{
    emit mouseMoveSignal(ev);
}

void LyricPanel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit mouseReleaseSignal(ev);
}

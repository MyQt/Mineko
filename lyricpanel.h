#ifndef LYRICPANEL_H
#define LYRICPANEL_H

#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QMouseEvent>

class LyricPanel : public QLabel
{
    Q_OBJECT

public:
    explicit LyricPanel(QWidget *parent = 0);
    ~LyricPanel();

protected:
    void mousePressEvent(QMouseEvent * ev);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void mousePressSignal(QMouseEvent* ev);
    void mouseDoubleClickSignal(QMouseEvent* ev);
    void mouseMoveSignal(QMouseEvent* ev);
    void mouseReleaseSignal(QMouseEvent* ev);
};

#endif // LYRICPANEL_H

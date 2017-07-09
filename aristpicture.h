#ifndef ARISTPICTURE_H
#define ARISTPICTURE_H

#include <QFrame>
#include <QTimer>
#include <QImage>

#include "channelswidget.h"

namespace Ui {
class AristPicture;
}

enum PLAYER_STATE
{
    STATE_STOP,
    STATE_PAUSE,
    STATE_PALYING
};

class AristPicture : public QFrame
{
    Q_OBJECT

public:
    explicit AristPicture(QWidget *parent = 0);
    ~AristPicture();

private:
    Ui::AristPicture *ui;
    QPixmap m_mixedPic;
    QPixmap m_mixedCurrentPic;
    QTimer *m_timer;
    QTimer *m_timerPic;
    QPixmap m_aristPic;
private:
    ushort m_iCurrentRotateAngle;
    int m_iCurrentState;
    qint64 m_totalTime;
    QString m_channelInfo;

public:
    void setAristPic(QPixmap pixmap);
    void saveAristPic(QString fileName);
    QPixmap loadAristPic(QString fileName);
    inline QPixmap& getAristPic() { return m_aristPic; }
public slots:
    void setDefultPic();
    void channelChangedSlot(QString channel);
    void setTotalTime(qint64 totalTime);
    void scrollInfo();
    void clickSlot();
private slots:
    void leaveSlot();
    void enterSlot();
    void showwhirlPicSlot();
signals:
    void pauseSignal(bool pause);
    void showPicSignal(qint64 position);
protected:
    void keyPressEvent(QKeyEvent *);

};

#endif // ARISTPICTURE_H

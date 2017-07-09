#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QMediaPlayer>
#include <QListWidgetItem>

#include "channelswidget.h"
#include "lyrics.h"
#include "volpannel.h"
#include "widget.h"
#include "aristpicture.h"
#include "getsongplaylistid.h"
#include "getsongreallink.h"
#include "getaristpic.h"
#include "getlrc.h"
#include "settingsdlg.h"

#include <QPair>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    enum e_play_mode
    {
        EPM_CIRCLE = 1,
        EPM_ORDER,
        EPM_ONE,
        EPM_RANDOM,
        EPM_END,
    };

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
public:
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void on_settingtoolButton_clicked();

    void on_voltoolButton_clicked();

    void on_showChanneltoolButton_clicked();

    void channelChangedSlot(CHANNEL_INFO channel);
    void on_nexttoolButton_clicked();

    void on_prevtoolButton_clicked();

    void pauseSlot(bool pause);
    void on_downloadtoolButton_clicked();

    void on_modeButton_clicked();

    void on_listBtn_clicked();

    void on_songListWidget_itemDoubleClicked(QListWidgetItem *item);


    void on_songListWidget_itemEntered(QListWidgetItem *item);
    void dir_changed_slot(QString filePath);
    void apply_skin_slot(QColor color);
    void dragLyricsSlot(qint64 position);
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::Widget *ui;
    void closeEvent(QCloseEvent *event);

private:
    QMediaPlayer *m_pPlayer;
    settingsDlg* m_setDlg;
    CHANNEL_INFO m_currentChannelId;
    QList <QString> m_songIdList;
    QList <QString> m_randIdList; // 随机播放列表
    QList <SONG_INFO> m_cachedSongList; // 缓存列表，当歌曲列表请求时间过长时，可从中取歌
    QMap <QString, QList<QString> > m_channelMap;
    QMap <QString, QList<SONG_INFO> > m_songMap;
    int m_iCurrentSongIndex;
    SONG_INFO m_currentSongInfo;

    QNetworkCookieJar *m_cookJar;
    int m_iCurrentMode;
    // 歌曲存放信息
    QString m_filePath;
    QColor m_bgColor;

    void playSong(SONG_INFO& songInfo);
    void play(int index);
    void nextSong();
    void prevSong();
    void randomSong();
    void loopOneSong();
    void orderSong();
    void initSongIdList(CHANNEL_INFO channel);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition();
    void volChannged(qint64 position);
    void modeLoad();
    void modeChanged();
    void setLabelTime(qint64 durMillionSec, qint64 totalMillionSec);
    void displaySongList(CHANNEL_INFO channel);
    void setBackgroundColor(QColor backColor);
    QString getButtonStyleSheet(QPalette& palette);
    QString getScrollBarStyleSheet();
    void playByMode();
signals:
    void channelInfoSignal(QString channel);
};

#endif // WIDGET_H

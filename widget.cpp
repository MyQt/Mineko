#include "widget.h"
#include "ui_widget.h"
#include "lyrics.h"
#include "aristpicture.h"
#include "downloadfile.h"
#include "songlistitem.h"
#include "loading.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QSettings>
#include <QDir>
#include <Qstyle>
#include <QColorDialog>
#include <QToolTip>
#include <QScrollBar>

#pragma execution_character_set("utf-8")


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_cookJar = new QNetworkCookieJar;
    m_setDlg = new settingsDlg(0);
    m_filePath = m_setDlg->getFilepath();
    m_pPlayer = new QMediaPlayer(this);
    m_iCurrentSongIndex = 0;
    ui->volSlider->hide();
    ui->channelwidget->hide();

    connect(this, SIGNAL(channelInfoSignal(QString)), ui->aristPicWidget, SLOT(channelChangedSlot(QString)));
    connect(ui->channelwidget, SIGNAL(channelChanged(CHANNEL_INFO)), this, SLOT(channelChangedSlot(CHANNEL_INFO)));
    connect(m_pPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::mediaStatusChanged);
    connect(m_pPlayer, &QMediaPlayer::positionChanged, this, &Widget::positionChanged);
    connect(m_pPlayer, &QMediaPlayer::durationChanged, this, &Widget::durationChanged);
    connect(m_pPlayer, SIGNAL(positionChanged(qint64)), ui->lycWidget, SLOT(setTickLrc(qint64)));
    connect(ui->lycWidget, SIGNAL(lyricsDragSignal(qint64)), this, SLOT(dragLyricsSlot(qint64)));
    connect(ui->volSlider,  &QSlider::valueChanged, this, &Widget::volChannged);
    connect(ui->progressSlider, &QSlider::sliderPressed, this, &Widget::setPosition);

    connect(ui->aristPicWidget, SIGNAL(pauseSignal(bool)), this, SLOT(pauseSlot(bool)));
    connect(m_pPlayer, SIGNAL(durationChanged(qint64)), ui->aristPicWidget, SLOT(setTotalTime(qint64)));
    connect(m_setDlg, SIGNAL(filepathChangeSlot(QString)), this, SLOT(dir_changed_slot(QString)));
    connect(m_setDlg, SIGNAL(applySkinSignal(QColor)), this, SLOT(apply_skin_slot(QColor)));
    //歌曲频道信息
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    int iChannel = settings.value("ChannelIndex").toInt();
    m_iCurrentMode = settings.value("play_mode").toInt();
    m_bgColor =settings.value("bgcolor").value<QColor>();
    int iVol = settings.value("volume").toInt();
    settings.endGroup();
    if (iVol == 0) iVol = 20;
    ui->volSlider->setRange(0, 100);
    ui->volSlider->setValue(iVol);
    if (m_iCurrentMode == 0)
    {
        m_iCurrentMode = EPM_CIRCLE;
    }
    modeLoad();

    if (!m_bgColor.isValid()) m_bgColor = QColor(125,0,125, 255);
    setBackgroundColor(m_bgColor);
    //获取频道列表
    ui->channelwidget->channelSelected(iChannel);
    // 设置播放列表透明
    ui->songListWidget->setStyleSheet("background-color:transparent");
    // 设置提示字体
    QFont font("Times", 10, QFont::Bold);
    QToolTip::setFont(font);
   \
    ui->lycWidget->installEventFilter(this);
}

void Widget::pauseSlot(bool pause)
{
    if(pause)
        m_pPlayer->pause();
    else
        m_pPlayer->play();
}

void Widget::volChannged(qint64 position)
{
    m_pPlayer->setVolume(position);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    settings.setValue("volume", position);
    settings.endGroup();
}
void Widget::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::EndOfMedia)
    {
        playByMode();
    }
}

void Widget::durationChanged(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);
    setLabelTime(0, duration);
}

void Widget::setPosition()
{
    m_pPlayer->setPosition(ui->progressSlider->value());
}

void Widget::positionChanged(qint64 position)
{
    ui->progressSlider->setValue(position);
    setLabelTime(position, ui->progressSlider->maximum());
}


void Widget::channelChangedSlot(CHANNEL_INFO channel)
{
    // 显示加载中
//    loading* loadUI = new loading(this);
//    loadUI->show();
    //初始化某个频道列表下的所有歌曲
    setEnabled(false);
    initSongIdList(channel);
    setEnabled(true);
    setFocus();
}


void Widget::initSongIdList(CHANNEL_INFO channel)
{
    m_songIdList.clear();
    m_randIdList.clear();
    m_iCurrentSongIndex = 0;
    m_currentChannelId.channelId = channel.channelId;
    m_currentChannelId.channelName = channel.channelName;
    QMap<QString, QList<QString> >::iterator iter = m_channelMap.find(channel.channelId);
    if (iter != m_channelMap.end()) // 列表已经加载过，则从映射中直接获取
    {
        m_songIdList = iter.value();
        m_randIdList = m_songIdList;
    } else
    {
        //第二步,获取某个频道下的歌曲ID列表
        GetSongPlayListId getPlayListId(this);
        m_songIdList = getPlayListId.getSongIdList(m_currentChannelId.channelId);
        m_randIdList = m_songIdList;
    }
    QString info = m_currentChannelId.channelName;
    emit channelInfoSignal(info);

    displaySongList(channel);
    info = m_currentChannelId.channelName + " 共有歌曲: " + QString::number(m_songIdList.size());
    emit channelInfoSignal(info);
}

Widget::~Widget()
{
    delete m_setDlg;
    delete ui;
}

void Widget::on_settingtoolButton_clicked()
{
    m_setDlg->showDlg();

}

void Widget::closeEvent(QCloseEvent *)
{
}

void Widget::on_voltoolButton_clicked()
{
    if(ui->volSlider->isHidden())
    {
        ui->volSlider->show();
        ui->volSlider->setSliderStyleSheet(m_bgColor, false);
    }
    else
        ui->volSlider->hide();
}

void Widget::on_prevtoolButton_clicked()
{
    playByMode();
}

void Widget::on_nexttoolButton_clicked()
{
    playByMode();
}



void Widget::on_showChanneltoolButton_clicked()
{
    if(ui->channelwidget->isHidden())
    {
        ui->channelwidget->show();
    }
    else
        ui->channelwidget->hide();
}


void Widget::prevSong()
{

    if(m_iCurrentSongIndex < 0)
    {
        return;
    } else if(m_iCurrentSongIndex > 0)
    {
        m_iCurrentSongIndex--;
    }
    play(m_iCurrentSongIndex);
}


void Widget::nextSong()
{
    if(m_iCurrentSongIndex < 0)
    {
        return;
    }
    if (m_iCurrentSongIndex < m_songIdList.size() - 1)
    {
        m_iCurrentSongIndex++;
    } else
    {
        m_iCurrentSongIndex = 0;
    }
    play(m_iCurrentSongIndex);
}

void Widget::loopOneSong()
{
    if (m_iCurrentSongIndex < 0)
    {
        return;
    }

    play(m_iCurrentSongIndex);
}

void Widget::randomSong()
{
    if (m_randIdList.size() <= 0) return;
    int index = qrand() % m_randIdList.size();
    m_randIdList.removeAt(index);
    m_iCurrentSongIndex = index;

    play(m_iCurrentSongIndex);
    if (m_randIdList.size() <= 0)
    {
        m_randIdList = m_songIdList;
    }
}

void Widget::orderSong()
{
    if (m_iCurrentSongIndex < 0 || m_iCurrentSongIndex >= m_songIdList.size()-1)
    {
        // 列表播放结束
        return;
    }

    m_iCurrentSongIndex++;

    play(m_iCurrentSongIndex);
}

void Widget::playSong(SONG_INFO& songInfo)
{
    if (songInfo.isEmpty())
        return;

    m_currentSongInfo = songInfo;
    QString fileName = m_filePath + "/" + songInfo.songName + ".mp3";
    QString fileLyric = m_filePath + "/lyrics/" + songInfo.songName + ".lrc";
    QString fileCover = m_filePath + "/Cover/" + songInfo.songName + ".png";
    QFile file;
    QString lrc;
    QPixmap picMap;
    if (file.exists(fileName))
    {
        m_pPlayer->setMedia(QUrl::fromLocalFile(fileName));
        lrc = ui->lycWidget->loadLyric(fileLyric);
        picMap = ui->aristPicWidget->loadAristPic(fileCover);
    } else
    {
        m_pPlayer->setMedia(QUrl(m_currentSongInfo.songRealLink));
    }
    m_pPlayer->play();
    if (lrc.isEmpty())
    {
        GetLrc getLrc;
        lrc = getLrc.getLrc(m_currentSongInfo.lrcLink);
    }
    if (picMap.isNull())
    {
        GetAristPic getPic;
        picMap = getPic.getAristPic(m_currentSongInfo.songPicRadio);

    }

    //获取歌词
    ui->lycWidget->currentSongChannged(m_currentSongInfo.songName, m_currentSongInfo.artistName, m_currentSongInfo.albumName);
    ui->lycWidget->setLrc(lrc);

    //显示歌手图片
    ui->aristPicWidget->setAristPic(picMap);
    // 歌曲列表滚动到当前行
    QListWidgetItem* pCurItem = ui->songListWidget->item(m_iCurrentSongIndex);
    ui->songListWidget->setItemSelected(pCurItem, true);
    ui->songListWidget->scrollToItem(pCurItem);
}


void Widget::play(int index)
{

    ui->aristPicWidget->setDefultPic();
    if(index > m_songIdList.size())
        return;
    int id = m_songIdList.at(index).toInt();
    QMap<QString, QList<SONG_INFO> >::iterator iter = m_songMap.find(m_currentChannelId.channelId);
    if (iter != m_songMap.end())
    {
        for (int i = 0; i < (*iter).size(); i++)
        {
            if ((*iter).at(i).id == id)
            {
                m_currentSongInfo = (*iter).at(i);
                break;
            }
        }
    } else
    {
        for (int i = 0; i < m_cachedSongList.size(); i++)
        {
            if (m_cachedSongList.at(i).id == id)
            {
                m_currentSongInfo = m_cachedSongList.at(i);
                break;
            }
        }
    }

    m_iCurrentSongIndex = index;
    playSong(m_currentSongInfo);
}


//下载歌曲
void Widget::on_downloadtoolButton_clicked()
{
    ui->downloadtoolButton->setEnabled(false);
    QString fileName = m_filePath;
    QString lyricName = m_filePath+"/lyrics/";
    QString coverName = m_filePath+"/Cover/";
    DownLoadFile downMp3;

    QDir dir;
    if(!dir.exists(fileName))
    {
        dir.mkdir(fileName);
    }
    if(!dir.exists(lyricName))
    {
        dir.mkdir(lyricName);
    }
    if(!dir.exists(coverName))
    {
        dir.mkdir(coverName);
    }

    fileName = m_filePath + "/" + m_currentSongInfo.songName + ".mp3";


    downMp3.getMp3File(fileName, m_currentSongInfo.songRealLink);

    lyricName = lyricName + m_currentSongInfo.songName + ".lrc";
    ui->lycWidget->saveLyric(lyricName);
    qDebug() << lyricName << "下载完成";
    coverName = coverName + m_currentSongInfo.songName + ".png";
    ui->aristPicWidget->saveAristPic(coverName);
    qDebug() << coverName << "下载完成";
    qDebug() << "下载完成";
    ui->downloadtoolButton->setEnabled(true);
    QToolTip::showText(ui->downloadtoolButton->mapToGlobal(ui->downloadtoolButton->pos()), "下载完成");
}

// 设置播放模式
void Widget::on_modeButton_clicked()
{
    modeChanged();
}

void Widget::modeLoad()
{
    QString strFile;
    switch (m_iCurrentMode) {
    case EPM_CIRCLE:
    {
        strFile = ":/img/loop_all.png";
        ui->modeButton->setToolTip("列表循环");
    }
        break;
    case EPM_ORDER:
        {
            strFile = ":/img/loop_no.png";
            ui->modeButton->setToolTip("顺序播放");
        }
        break;
    case EPM_ONE:
        {
            strFile = ":/img/loop_one.png";
            ui->modeButton->setToolTip("单曲循环");
        }
        break;
    case EPM_RANDOM:
        {
            strFile = ":/img/loop_random.png";
            ui->modeButton->setToolTip("随机播放");
        }
        break;
    default:
        break;
    }
    ui->modeButton->setIcon(QIcon(strFile));
}

void Widget::modeChanged()
{
    if (m_iCurrentMode + 1 != EPM_END)
    {
        m_iCurrentMode = m_iCurrentMode+1;
    } else
    {
        m_iCurrentMode = EPM_CIRCLE;
    }
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    settings.setValue("play_mode", m_iCurrentMode);
    settings.endGroup();
    modeLoad();
}

void Widget::setLabelTime(qint64 durMillionSec, qint64 totalMillionSec)
{
    QString curMinute = QString::number(durMillionSec / 60000, 10); // 分钟
    QString curSec = QString::number((durMillionSec - curMinute.toInt() * 60 * 1000) / 1000, 10);

    QString endMinute = QString::number(totalMillionSec / 60000, 10); // 结束分钟数
    QString endSec = QString::number((totalMillionSec - endMinute.toInt() * 60 * 1000) / 1000, 10);

    QString strText;

    strText += curMinute+":"+curSec;
    strText += " / ";
    strText += endMinute+":"+endSec;
    ui->labelTime->setText(strText);
}

void Widget::displaySongList(CHANNEL_INFO channel)
{
    ui->songListWidget->clear();
    m_cachedSongList.clear();
    SONG_INFO currentSongInfo;
    QMap<QString, QList<SONG_INFO> >::iterator iter = m_songMap.find(channel.channelId);
    QToolTip::showText(QPoint(x()+width()/2, y()+height()/2), "播放列表加载中...");
    if (iter != m_songMap.end())
    {
        for (int i = 0; i < iter.value().size(); i++)
        {
            currentSongInfo = iter.value().at(i);
            SongListItem* pItem = new SongListItem(ui->songListWidget);
            pItem->setGeometry(0,0, ui->songListWidget->rect().width(), ui->songListWidget->rect().width()/3);
            QString fileCover = m_filePath + "/Cover/" + currentSongInfo.songName + ".png";
            QFile file;
            QPixmap picMap;
            if (file.exists(fileCover))
            {
                picMap = ui->aristPicWidget->loadAristPic(fileCover);
            } else
            {
                GetAristPic getPic;
                picMap = getPic.getAristPic(currentSongInfo.songPicRadio);
            }

            pItem->displayItem(i, picMap, currentSongInfo.artistName, currentSongInfo.songName);
            pItem->show();

            QListWidgetItem* pItemWidget = new QListWidgetItem();
            ui->songListWidget->addItem(pItemWidget);
            pItemWidget->setSizeHint(QSize(pItem->rect().width(),pItem->rect().height()));
            ui->songListWidget->setItemWidget(pItemWidget, pItem);
            if (i == 0)
            {
                m_iCurrentSongIndex = 0;
                playSong(currentSongInfo);
            }
        }

    } else
    {
        QList<SONG_INFO> songInfoList;
        songInfoList.clear();
        GetSongRealLink getSongLink(this);

        for (int i = 0; i < m_songIdList.size(); i++)
        {
            currentSongInfo = getSongLink.getSongRealLinkById(m_songIdList.at(i));
            if (currentSongInfo.isEmpty())
            {
                m_songIdList.removeAt(i);
                m_randIdList.removeAt(i);
                i--;
            } else
            {
                songInfoList.push_back(currentSongInfo);
                SongListItem* pItem = new SongListItem(ui->songListWidget);
                pItem->setGeometry(0,0, ui->songListWidget->rect().width(), ui->songListWidget->rect().width()/3);
                QString fileCover = m_filePath + "/Cover/" + currentSongInfo.songName + ".png";
                QFile file;
                QPixmap picMap;
                if (file.exists(fileCover))
                {
                    picMap = ui->aristPicWidget->loadAristPic(fileCover);
                } else
                {
                    GetAristPic getPic;
                    picMap = getPic.getAristPic(currentSongInfo.songPicRadio);
                }

                pItem->displayItem(i, picMap, currentSongInfo.artistName, currentSongInfo.songName);
                pItem->show();

                QListWidgetItem* pItemWidget = new QListWidgetItem();
                ui->songListWidget->addItem(pItemWidget);
                pItemWidget->setSizeHint(QSize(pItem->rect().width(),pItem->rect().height()));
                ui->songListWidget->setItemWidget(pItemWidget, pItem);
                m_cachedSongList.push_back(currentSongInfo);
                if (i == 0)
                {
                    m_iCurrentSongIndex = 0;
                    playSong(currentSongInfo);
                }
            }
        }
        m_songMap.insert(channel.channelId, songInfoList);
        m_channelMap.insert(channel.channelId, m_songIdList);
        m_cachedSongList.clear();
    }

    QToolTip::hideText();
    if (m_songIdList.isEmpty())
    {
        ui->songListWidget->hide();
        return;
    }
}

void Widget::on_listBtn_clicked()
{
    if(ui->songListWidget->isHidden() && !m_songIdList.isEmpty())
    {
        ui->songListWidget->show();
        QIcon ico(":/img/fold.png");
        ui->listBtn->setIcon(ico);
    }
    else
    {
        QIcon ico(":/img/expand.png");
        ui->listBtn->setIcon(ico);
        ui->songListWidget->hide();
    }
}

void Widget::on_songListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    SongListItem* pItem = static_cast<SongListItem*>(ui->songListWidget->itemWidget(item));
    if (pItem)
    {
        play(pItem->getId());
    }
}


void Widget::setBackgroundColor(QColor backColor)
{
    QPalette palette;
    palette.setColor(QPalette::Window, backColor);
    setPalette(palette);
    ui->songListWidget->setPalette(palette);

    // 设置歌词颜色为背景反色
    QPalette paletteLyric(palette);
    QColor colorLyric = QColor(255-backColor.red(), 255-backColor.green(), 255-backColor.blue(),backColor.alpha());
    paletteLyric.setColor(QPalette::WindowText, colorLyric);
    ui->lycWidget->setPalette(paletteLyric);
    ui->lycWidget->setLyricPalette(palette);
    // 设置封面轮盘
    ui->aristPicWidget->setPalette(palette);
    // 设置提示颜色
    QToolTip::setPalette(paletteLyric);
    // 设置滚动条样式
    ui->songListWidget->horizontalScrollBar()->setStyleSheet(getScrollBarStyleSheet());
    ui->songListWidget->verticalScrollBar()->setStyleSheet(getScrollBarStyleSheet());
    // 设置滑动篥样式
    ui->progressSlider->setSliderStyleSheet(m_bgColor, true);
    ui->volSlider->setSliderStyleSheet(m_bgColor, false);
    // 设置频道按钮样式
    ui->channelwidget->bgColorChanged(backColor);
    show();
}


void Widget::on_songListWidget_itemEntered(QListWidgetItem *item)
{
    SongListItem* pItem = static_cast<SongListItem*>(ui->songListWidget->itemWidget(item));
    if (pItem)
    {
        if(pItem->getId() > m_songIdList.size())
            return;
        int id = m_songIdList.at(pItem->getId()).toInt();
        QMap<QString, QList<SONG_INFO> >::iterator iter = m_songMap.find(m_currentChannelId.channelId);
        if (iter == m_songMap.end())
        {
            return;
        }
        SONG_INFO songInfo;
        for (int i = 0; i < (*iter).size(); i++)
        {
            if ((*iter).at(i).id == id)
            {
                songInfo = (*iter).at(i);
                break;
            }
        }
        if (!songInfo.isEmpty())
        {
            // 悬浮显示专辑名/时长/大小/码率
            QString songTips = "专辑名: ";
            if (songInfo.albumName.isEmpty())
                songTips.append("未知");
            else
                songTips.append(songInfo.albumName);
            songTips.append("\r\n");
            songTips.append("歌曲长度: ");
            if (songInfo.time.isEmpty())
                songTips.append("未知");
            else
            {
                int time = songInfo.time.toInt();

                QString minute = QString::number(time / 60, 10);
                QString second = QString::number(time % 60, 10);

                songTips.append(minute);
                songTips.append(":");
                songTips.append(second);
            }
            songTips.append("\r\n");
            songTips.append("歌曲大小: ");
            if (songInfo.size.isEmpty())
                songTips.append("未知");
            else
            {
                songTips.append(songInfo.size);
                songTips.append("MB");
            }
            songTips.append("\r\n");
            songTips.append("歌曲采样率: ");
            if (songInfo.rate.isEmpty())
                songTips.append("未知");
            else
            {
                songTips.append(songInfo.rate);
                songTips.append("kbps");
            }

            pItem->setToolTip(songTips);

        }
    }
}

void Widget::dir_changed_slot(QString filePath)
{
    m_filePath = filePath;
}

void Widget::apply_skin_slot(QColor color)
{

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    settings.setValue("bgcolor", color);
    settings.endGroup();
    m_bgColor = color;
    setBackgroundColor(color);
}

void Widget::dragLyricsSlot(qint64 position)
{
    m_pPlayer->setPosition(position);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        ui->lycWidget->setFullScreen();
        hide();
    } else if (event->key() == Qt::Key_Up)
    {
        ui->volSlider->setValue(ui->volSlider->value()+5);
    } else if (event->key() == Qt::Key_Down)
    {
        ui->volSlider->setValue(ui->volSlider->value()-5);
    } else if (event->key() == Qt::Key_Left)
    {
        ui->progressSlider->setValue(ui->progressSlider->value()-5000); // 5秒
        setLabelTime(ui->progressSlider->value(), ui->progressSlider->maximum());
    } else if (event->key() == Qt::Key_Right)
    {
        ui->progressSlider->setValue(ui->progressSlider->value()+5000); // 5秒
        setLabelTime(ui->progressSlider->value(), ui->progressSlider->maximum());
    }
}

QString Widget::getButtonStyleSheet(QPalette& palette)
{
    QColor color = palette.color(QPalette::Window);
    QColor arcColor = QColor(255-color.red(), 255-color.green(), 255-color.blue(),color.alpha());
    QString styleSheet;
    styleSheet.append("QPushButton{border-style: none;");
    styleSheet.append("background-color:").append(color.name()).append(";");
    styleSheet.append("color:").append(arcColor.name()).append(";");
    styleSheet.append("}");

    return styleSheet;
}

QString Widget::getScrollBarStyleSheet()
{
    QString styleSheet;
    styleSheet.append("QScrollBar::handle{background:").append(m_bgColor.name());
    styleSheet.append(";border:2px solid transparent; border-radius:5px; }");
    styleSheet.append("QScrollBar::handle:hover{background:").append(m_bgColor.lighter(50).name()).append("; }");
    styleSheet.append("QScrollBar::handle:pressed{background:").append(m_bgColor.darker(300).name()).append("; }");
    styleSheet.append("QScrollBar::sub-line{background:transparent;}");
    styleSheet.append("QScrollBar::add-line{background:transparent;}");

    return styleSheet;
}

void Widget::playByMode()
{
    switch (m_iCurrentMode) {
    case EPM_CIRCLE:
        nextSong();
        break;
    case EPM_ONE:
        loopOneSong();
        break;
    case EPM_ORDER:
        orderSong();
        break;
    case EPM_RANDOM:
        randomSong();
        break;
    default:
        break;
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lycWidget)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape)
            {
                show();
                setFocus();
            } else if (keyEvent->key() == Qt::Key_Up)
            {
                ui->volSlider->setValue(ui->volSlider->value()+5);
            } else if (keyEvent->key() == Qt::Key_Down)
            {
                ui->volSlider->setValue(ui->volSlider->value()-5);
            } else if (keyEvent->key() == Qt::Key_Left)
            {
                ui->progressSlider->setValue(ui->progressSlider->value()-5000); // 5秒
                setLabelTime(ui->progressSlider->value(), ui->progressSlider->maximum());
            } else if (keyEvent->key() == Qt::Key_Right)
            {
                ui->progressSlider->setValue(ui->progressSlider->value()+5000); // 5秒
                setLabelTime(ui->progressSlider->value(), ui->progressSlider->maximum());
            } else if (keyEvent->key() == Qt::Key_Space)
            {
                ui->aristPicWidget->clickSlot();
            }
        }
    }

    return false;
}

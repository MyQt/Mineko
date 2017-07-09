#include "songlistitem.h"
#include <QDebug>

SongListItem::SongListItem(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    m_pic = new QLabel(this);
    m_singer = new QLabel(this);
    m_song = new QLabel(this);

    m_pic->setAlignment(Qt::AlignCenter);
    m_singer->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    m_song->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    m_pic->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_singer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_song->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_pic->setMouseTracking(true);
    m_singer->setMouseTracking(true);
    m_song->setMouseTracking(true);
}

void SongListItem::displayItem(int nIndex, QPixmap &picRadio, QString &aristName, QString &songName)
{
    m_nIndex = nIndex;
    m_pic->setGeometry(0,0,rect().width()/3, rect().height());
    m_singer->setGeometry(rect().width()/3+1,0,rect().width()*2/3-1,rect().height()/2);
    m_song->setGeometry(rect().width()/3+1,rect().height()/2+1,rect().width()*2/3-1,rect().height()/2-1);
    if (picRadio.isNull())
    {
       QPixmap picDefault(":/img/cd.png");
       picDefault = picDefault.scaled(m_pic->width(), m_pic->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
       m_pic->setPixmap(picDefault);
    } else
    {
        picRadio = picRadio.scaled(m_pic->width(), m_pic->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_pic->setPixmap(picRadio);
    }
    m_singer->setText(aristName);
    m_song->setText(songName);
}

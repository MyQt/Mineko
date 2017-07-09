#include "lyrics.h"
#include "ui_lyrics.h"

#include <QRegExp>
#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QDesktopWidget>

Lyrics::Lyrics(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Lyrics)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::NoFrame);
    ui->label->setWordWrap(true);
    ui->label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_iCurrentIndex = 0;

    connect(ui->label, SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mousePressEventSlot(QMouseEvent*)));
    connect(ui->label, SIGNAL(mouseDoubleClickSignal(QMouseEvent*)), this, SLOT(mouseDoubleClickEventSlot(QMouseEvent*)));
    connect(ui->label, SIGNAL(mouseMoveSignal(QMouseEvent*)), this, SLOT(mouseMoveEventSlot(QMouseEvent*)));
    connect(ui->label, SIGNAL(mouseReleaseSignal(QMouseEvent*)), this, SLOT(mouseReleaseEventSlot(QMouseEvent*)));
}

Lyrics::~Lyrics()
{
    delete ui;
}

void Lyrics::currentSongChannged(QString songName, QString aristName, QString ablumName)
{
    m_songName = songName;
    m_aristName = aristName;
    m_ablumName = ablumName;

    QString lrc;
    lrc.clear();
    lrc += QString("<p align='center'><span style=' font-style:italic;line-height:20px'>");
    lrc += m_aristName + "--" + m_songName;
    lrc += QString("</span></p>");
    ui->label->setText(lrc);

}

void Lyrics::setLrc(QString lrc)
{
    m_lyricText = lrc;
    QTextStream stream(&lrc);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    stream.setCodec(codec);

    m_iCurrentIndex = 0;
    m_lyric.clear();

    m_lyric = parse(stream);
}

QList<QLyric> Lyrics::parse(QTextStream &stream)
{
    QRegExp timeExp;
    timeExp.setPatternSyntax(QRegExp::RegExp);
    timeExp.setCaseSensitivity(Qt::CaseSensitive);
    timeExp.setPattern("\\[([0-9]{2}):([0-9]{2})\\.([0-9]{2})\\]");
    QList<QLyric> result;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        int ret = timeExp.indexIn(line);
        QList<QTime> ticks;
        int lastindex = 0;
        while (ret >= 0) {
            QStringList tstr = timeExp.capturedTexts();
            QTime time(0, tstr[1].toInt(), tstr[2].toInt(), tstr[3].toInt());
            ticks.append(time);
            lastindex = ret + timeExp.matchedLength();
            ret = timeExp.indexIn(line, lastindex);
        }
        QString lyricstr = line.right(line.size() - lastindex);
        for (const QTime& t : ticks) {
            QLyric lyric;
            lyric.time = t.minute() * 60 * 1000 + t.second() * 1000 + t.msec();

            lyric.lyric = lyricstr;

            result.append(lyric);
        }
    }
    std::sort(result.begin(), result.end(), [] (const QLyric& a, const QLyric& b) -> bool {
        return a.time < b.time;
    });

    return result;
}


void Lyrics::setTickLrc(qint64 position)
{
    if(m_lyric.size() <= 0 || m_iCurrentIndex >= m_lyric.size())
        return;


    for(int i = 0; i < m_lyric.size(); i++)
    {

        if(position > m_lyric.at(i).time)
        {
            m_iCurrentIndex = i;
        }
    }


    if(position > m_lyric.at(m_iCurrentIndex).time)
    {
        QString lrc;
        lrc.clear();
        lrc += QString("<p align='center'><span style=' font-style:italic;line-height:20px'>");
        lrc += m_aristName + " - " + m_songName;
        lrc += QString("</span></p>");

        for(int j = m_iCurrentIndex - 1; j < m_lyric.size(); j++)
        {
            if(j < 0)
                continue;

            if(m_iCurrentIndex == j)
            {
                QPalette palette = ui->label->palette();
                QColor colorLight = palette.color(QPalette::WindowText).lighter(50);
                lrc += QString("<p align='center'><span style=' font-weight:600;line-height:20px; color:");
                lrc +=colorLight.name().append("'>");
                if (!m_lyric.at(m_iCurrentIndex).lyric.isEmpty())
                    lrc += ("@");
                lrc += m_lyric.at(m_iCurrentIndex).lyric;
                if (!m_lyric.at(m_iCurrentIndex).lyric.isEmpty())
                    lrc += "@";
                lrc += QString("</span></p>");
            }
            else
            {
                lrc += "<p align='center'><span styele='line-height:18px;'>";
                lrc += m_lyric.at(j).lyric;
                lrc += "</p>";
            }


        }

        ui->label->setText(lrc);
        m_iCurrentIndex++;
    }
}

void Lyrics::setLyricPalette(QPalette& palette)
{
    ui->label->setPalette(palette);

}
// -----------------------
// 歌词文件保存为.lrc格式
// -----------------------
void Lyrics::saveLyric(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.exists())
    {
        bool bOk = file.open(QIODevice::WriteOnly);
        if(bOk && !m_lyricText.isEmpty())
        {
            file.write(m_lyricText.toUtf8());
            file.close();
        }
    }
}

QString Lyrics::loadLyric(QString fileName)
{
    QString lyric;
    QFile file;
    file.setFileName(fileName);
    if (file.exists())
    {
        bool bOk = file.open(QIODevice::ReadOnly);
        if(bOk)
        {
            QByteArray byteArray = file.readAll();
            lyric = byteArray.data();
        }
    }

    return lyric;
}

void Lyrics::setBackgroundPic(QPalette palette)
{
    ui->label->setPalette(palette);
}

void Lyrics::mousePressEventSlot(QMouseEvent *event)
{
    if (ui->label->geometry().contains(event->pos()))
    {
        m_pressPos = event->pos();
    }
}

void Lyrics::mouseDoubleClickEventSlot(QMouseEvent *event)
{
    if (ui->label->geometry().contains(event->pos()))
    {
        m_pressPos = event->pos();
        // 根据高度差设置歌词位置变化
        int iOffsetY = m_pressPos.y() - ui->label->pos().y();
        int iDragTimes = iOffsetY/(ui->label->height()/10)-1;

        if (iDragTimes != 0)
        {
            m_iCurrentIndex += iDragTimes;
            if (m_iCurrentIndex <= 0) {
                m_iCurrentIndex = 1;
            } else if(m_iCurrentIndex >= m_lyric.size())
            {
                m_iCurrentIndex = m_lyric.size() - 1;
            }
            setTickLrc(m_lyric.at(m_iCurrentIndex).time);
            emit lyricsDragSignal(m_lyric.at(m_iCurrentIndex).time);
        }
        m_pressPos = QPoint(0,0);
    }
}

void Lyrics::mouseMoveEventSlot(QMouseEvent *event)
{
    if (ui->label->geometry().contains(event->pos()) && event->buttons() == Qt::LeftButton)
    {
        m_releasePos = event->pos();
        // 根据高度差设置歌词位置变化
        int iOffsetY = m_releasePos.y() - m_pressPos.y();
        int iDragTimes = iOffsetY/(ui->label->height()/10)-1;

        if (iDragTimes != 0)
        {
            m_iCurrentIndex -= iDragTimes;
            if (m_iCurrentIndex <= 0) {
                m_iCurrentIndex = 1;
            } else if(m_iCurrentIndex >= m_lyric.size())
            {
                m_iCurrentIndex = m_lyric.size() - 1;
            }
            setTickLrc(m_lyric.at(m_iCurrentIndex).time);
        }
    }
}

void Lyrics::mouseReleaseEventSlot(QMouseEvent *event)
{
    if (ui->label->geometry().contains(event->pos()))
    {
        m_releasePos = event->pos();
        // 根据高度差设置歌词位置变化
        int iOffsetY = m_releasePos.y() - m_pressPos.y();
        int iDragTimes = iOffsetY/(ui->label->height()/10)-1;
        if (iDragTimes != 0)
        {
            m_iCurrentIndex -= iDragTimes;
            if (m_iCurrentIndex <= 0) {
                m_iCurrentIndex = 1;
            } else if(m_iCurrentIndex >= m_lyric.size())
            {
                m_iCurrentIndex = m_lyric.size() - 1;
            }
            setTickLrc(m_lyric.at(m_iCurrentIndex).time);
            emit lyricsDragSignal(m_lyric.at(m_iCurrentIndex).time);
        }
    }
}

void Lyrics::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (isFullScreen())
        {
            QDesktopWidget *pDesktop = QApplication::desktop();
            pDesktop->hide();
            setWindowFlags(Qt::SubWindow);
            showNormal();
            setWindowOpacity(0);
            ui->label->resize(width(), height());
            QFont fontLyric = ui->label->font();
            fontLyric.setPointSize(fontLyric.pointSize()/2);
            ui->label->setFont(fontLyric);
            setFrameStyle(QFrame::NoFrame);
            setFrameShadow(QFrame::Raised);
        }
    }
}

void Lyrics::setFullScreen()
{
    if (!isFullScreen())
    {
        QDesktopWidget *pDesktop = QApplication::desktop();;
        pDesktop->show();
        setWindowFlags(Qt::Window);
        showFullScreen();
        setWindowOpacity(0.5);
        QFont fontLyric = ui->label->font();
        fontLyric.setPointSize(fontLyric.pointSize()*2);
        ui->label->setFont(fontLyric);
        ui->label->resize(width(), height());
        setFrameStyle(QFrame::StyledPanel);
        setFrameShadow(QFrame::Raised);
    }
}

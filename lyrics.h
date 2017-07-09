#ifndef LYRICS_H
#define LYRICS_H

#include <QFrame>
#include <QTextStream>
#include <QTextCodec>
#include <QTime>
#include <QMouseEvent>

namespace Ui {
class Lyrics;
}

struct QLyric
{
    qint64 time;
    QString lyric;
};

class Lyrics : public QFrame
{
    Q_OBJECT

public:
    explicit Lyrics(QWidget *parent = 0);
    ~Lyrics();
    QString loadLyric(QString fileName);
private slots:
    void mousePressEventSlot(QMouseEvent *event);
    void mouseDoubleClickEventSlot(QMouseEvent* event);
    void mouseMoveEventSlot(QMouseEvent *event);
    void mouseReleaseEventSlot(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::Lyrics *ui;
    QList<QLyric> parse(QTextStream &stream);
    QList<QLyric> m_lyric;

    QString m_songName;
    QString m_aristName;
    QString m_ablumName;
    QString m_lyricText;

    int m_iCurrentIndex;
    QPoint m_pressPos;
    QPoint m_releasePos;
    QRect  m_original;
public:
    void setLrc(QString lrc);

    void currentSongChannged(QString songName, QString aristName, QString ablumName);
    void setLyricPalette(QPalette& palette);
    void saveLyric(QString fileName);
    void setBackgroundPic(QPalette palette);
    void setFullScreen();
public slots:
    void setTickLrc(qint64 position);
signals:
    void lyricsDragSignal(qint64 position);
};

#endif // LYRICS_H

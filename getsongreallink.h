#ifndef GETSONGREALLINK_H
#define GETSONGREALLINK_H

#include <QObject>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkCookieJar>

typedef struct _SONG_INFO
{
    int     id;
    QString songRealLink;
    QString songName;
    QString artistName;
    QString songPicBig;
    QString songPicSmall;
    QString songPicRadio;
    QString albumName;
    QString lrcLink;

    QString time;
    QString rate; // 歌曲采样率
    QString size; // 歌曲大小，转换为MB

    bool isEmpty() {
        return songRealLink.isEmpty() || songName.isEmpty();
    }
}SONG_INFO;

class GetSongRealLink : public QObject
{
    Q_OBJECT
public:
    explicit GetSongRealLink(QObject *parent = 0, QNetworkCookieJar *cookie = NULL);

signals:

public slots:

    SONG_INFO getSongRealLinkById(QString songId);

private:
    QNetworkCookieJar *m_cookJar;
};

#endif // GETSONGREALLINK_H

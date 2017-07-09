#include "getsongreallink.h"

const QString downLoadLinkUrl = "http://music.baidu.com/data/music/fmlink?type=mp3&rate=320&songIds=";

GetSongRealLink::GetSongRealLink(QObject *parent, QNetworkCookieJar *cookie) : QObject(parent)
{
    m_cookJar = cookie;
}

SONG_INFO GetSongRealLink::getSongRealLinkById(QString songId)
{
    QNetworkRequest networkRequest;
    QNetworkAccessManager networkManager;
    networkRequest.setUrl(QUrl(downLoadLinkUrl + songId));
    networkManager.setCookieJar(m_cookJar);
    QNetworkReply *reply = networkManager.get(networkRequest);
    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redirect.isEmpty())
    {
        networkRequest.setAttribute(QNetworkRequest::RedirectionTargetAttribute, true);
        networkRequest.setUrl(redirect);
        reply = networkManager.get(networkRequest);
    }
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    reply->deleteLater();


    QByteArray buf = reply->readAll();

    QJsonParseError jsonError;
    QJsonDocument jsonRes = QJsonDocument::fromJson(buf, &jsonError);

    //歌曲信息
    SONG_INFO songInfo;
    songInfo.id = songId.toInt();
    if(jsonError.error == QJsonParseError::NoError)
    {
        if(jsonRes.isObject())
        {
            QVariantMap result = jsonRes.toVariant().toMap();
            QVariantMap dataList = result["data"].toMap();

            foreach(QVariant data, dataList["songList"].toList())
            {
                QVariantMap linkMap = data.toMap();


                songInfo.songRealLink = linkMap["songLink"].toString();
                songInfo.songName = linkMap["songName"].toString();
                songInfo.artistName = linkMap["artistName"].toString();
                songInfo.songPicBig = linkMap["songPicBig"].toString();
                songInfo.songPicSmall = linkMap["songPicSmall"].toString();
                songInfo.songPicRadio = linkMap["songPicRadio"].toString();
                songInfo.albumName = linkMap["albumName"].toString();
                songInfo.lrcLink = linkMap["lrcLink"].toString();
                songInfo.time = linkMap["time"].toString(); // 获取单位为秒
                songInfo.rate = linkMap["rate"].toString();
                songInfo.size = QString::number(linkMap["size"].toDouble()/1024/1024,'g',2); // 转换为MB
            }

        }
    }

    return songInfo;
}


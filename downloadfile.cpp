#include "downloadfile.h"

DownLoadFile::DownLoadFile(QObject *parent) : QObject(parent)
{

}

void DownLoadFile::getMp3File(QString fileName, QString url)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.exists())
    {
        QNetworkRequest networkRequest;
        QNetworkAccessManager networkManager;

        networkRequest.setUrl(QUrl(url));

        QNetworkReply *reply = networkManager.get(networkRequest);

        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        reply->deleteLater();

        bool bOk = file.open(QIODevice::WriteOnly);
        if(bOk)
        {
            file.write(reply->readAll());
            file.close();
        }
    }
}


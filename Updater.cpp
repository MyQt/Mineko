/*
 * Copyright (c) 2014-2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the QSimpleUpdater library, which is released under
 * the DBAD license, you can read a copy of it below:
 *
 * DON'T BE A DICK PUBLIC LICENSE TERMS AND CONDITIONS FOR COPYING,
 * DISTRIBUTION AND MODIFICATION:
 *
 * Do whatever you like with the original work, just don't be a dick.
 * Being a dick includes - but is not limited to - the following instances:
 *
 * 1a. Outright copyright infringement - Don't just copy this and change the
 *     name.
 * 1b. Selling the unmodified original with no work done what-so-ever, that's
 *     REALLY being a dick.
 * 1c. Modifying the original work to contain hidden harmful content.
 *     That would make you a PROPER dick.
 *
 * If you become rich through modifications, related works/services, or
 * supporting the original work, share the love.
 * Only a dick would make loads off this work and not buy the original works
 * creator(s) a pint.
 *
 * Code is provided with no warranty. Using somebody else's code and bitching
 * when it goes wrong makes you a DONKEY dick.
 * Fix the problem yourself. A non-dick would submit the fix back.
 */

#include <QJsonValue>
#include <QJsonObject>
#include <QMessageBox>
#include <QApplication>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QtMath>
#include <QDir>
#include <QUrl>

#include "Updater.h"

#pragma execution_character_set("utf-8")

static const QString PARTIAL_DOWN (".part");
static const QDir DOWNLOAD_DIR (QDir::homePath() + "/Downloads/");

Updater::Updater() {
    m_url = "";
    m_openUrl = "";
    m_changelog = "";
    m_downloadUrl = "";
    m_latestVersion = "";
    m_filename = "";
    m_startTime = 0;
    m_customAppcast = false;
    m_notifyOnUpdate = true;
    m_notifyOnFinish = false;
    m_updateAvailable = false;
    m_downloaderEnabled = true;
    m_moduleName = qApp->applicationName();
    m_moduleVersion = qApp->applicationVersion();
    m_manager = new QNetworkAccessManager();

#if defined Q_OS_WIN
    m_platform = "windows";
#elif defined Q_OS_MAC
    m_platform = "osx";
#elif defined Q_OS_LINUX
    m_platform = "linux";
#elif defined Q_OS_ANDROID
    m_platform = "android";
#elif defined Q_OS_IOS
    m_platform = "ios";
#endif
    connect (m_manager,    SIGNAL (finished (QNetworkReply*)),
             this,           SLOT (onReply  (QNetworkReply*)));
}

Updater::~Updater() {
}

QString Updater::url() const {
    return m_url;
}

QString Updater::openUrl() const {
    return m_openUrl;
}

QString Updater::changelog() const {
    return m_changelog;
}

/**
 * Returns the name of the module (if defined)
 */
QString Updater::moduleName() const {
    return m_moduleName;
}

/**
 * Returns the platform key (be it system-set or user-set).
 * If you do not define a platform key, the system will assign the following
 * platform key:
 *    - On iOS: \c ios
 *    - On Mac OSX: \c osx
 *    - On Android: \c android
 *    - On GNU/Linux: \c linux
 *    - On Microsoft Windows: \c windows
 */
QString Updater::platformKey() const {
    return m_platform;
}

QString Updater::downloadUrl() const {
    return m_downloadUrl;
}

QString Updater::latestVersion() const {
    return m_latestVersion;
}

QString Updater::moduleVersion() const {
    return m_moduleVersion;
}

/**
 * Returns \c true if the updater should NOT interpret the downloaded appcast.
 * This is useful if you need to store more variables (or information) in the
 * JSON file or use another appcast format (e.g. XML)
 */
bool Updater::customAppcast() const {
    return m_customAppcast;
}

bool Updater::notifyOnUpdate() const {
    return m_notifyOnUpdate;
}

bool Updater::notifyOnFinish() const {
    return m_notifyOnFinish;
}

bool Updater::updateAvailable() const {
    return m_updateAvailable;
}

bool Updater::downloaderEnabled() const {
    return m_downloaderEnabled;
}

bool Updater::useCustomInstallProcedures() const {
    return m_useCustomProcedures;
}

/**
 * Downloads and interpets the update definitions file referenced by the
 * \c url() function.
 */
void Updater::checkForUpdates(const QString& url) {
    m_manager->get (QNetworkRequest (QUrl(url)));
}

void Updater::setUrl (const QString& url) {
    m_url = url;
}

/**
 * Changes the module \a name.
 * \note The module name is used on the user prompts. If the module name is
 *       empty, then the prompts will show the name of the application.
 */
void Updater::setModuleName (const QString& name) {
    m_moduleName = name;
}

void Updater::setNotifyOnUpdate (const bool notify) {
    m_notifyOnUpdate = notify;
}

void Updater::setNotifyOnFinish (const bool notify) {
    m_notifyOnFinish = notify;
}

void Updater::setModuleVersion (const QString& version) {
    m_moduleVersion = version;
}

void Updater::setDownloaderEnabled (const bool enabled) {
    m_downloaderEnabled = enabled;
}

void Updater::setPlatformKey (const QString& platformKey) {
    m_platform = platformKey;
}

void Updater::setUseCustomAppcast (const bool customAppcast) {
    m_customAppcast = customAppcast;
}

void Updater::setUseCustomInstallProcedures (const bool custom) {
    m_useCustomProcedures = custom;
}

/**
 * Called when the download of the update definitions file is finished.
 */
void Updater::onReply (QNetworkReply* reply) {
    /* Check if we need to redirect */
    QUrl redirect = reply->attribute (
                        QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redirect.isEmpty()) {
        setUrl (redirect.toString());
        checkForUpdates(redirect.toString());
        return;
    }

    /* There was a network error */
    if (reply->error() != QNetworkReply::NoError)
        return;

    /* The application wants to interpret the appcast by itself */
    if (customAppcast()) {
        emit appcastDownloaded (url(), reply->readAll());
        return;
    }

    /* Try to create a JSON document from downloaded data */
    QJsonDocument document = QJsonDocument::fromJson (reply->readAll());

    /* JSON is invalid */
    if (document.isNull())
        return;

    /* Get the platform information */
    QJsonObject updates = document.object().value ("updates").toObject();
    QJsonObject platform = updates.value (platformKey()).toObject();

    /* Get update information */
    m_openUrl = platform.value ("open-url").toString();
    m_changelog = platform.value ("changelog").toString();
    m_downloadUrl = platform.value ("download-url").toString();
    m_latestVersion = platform.value ("latest-version").toString();

    /* Compare latest and current version */
    setUpdateAvailable (compare (latestVersion(), moduleVersion()));
    emit checkingFinished (url());
}

/**
 * Prompts the user based on the value of the \a available parameter and the
 * settings of this instance of the \c Updater class.
 */
void Updater::setUpdateAvailable (const bool available) {
    m_updateAvailable = available;

    QMessageBox box;
    box.setTextFormat (Qt::RichText);
    box.setIcon (QMessageBox::Information);

    if (updateAvailable() && (notifyOnUpdate() || notifyOnFinish())) {
        QString text = "要马上下载更新吗?";
        QString title = "<h3>"
                        + QString("新版本 %1 of %2 已经发布啦!").arg (latestVersion()).arg (moduleName())
                        + "</h3>";

        box.setText (title);
        box.setInformativeText (text);
        box.setStandardButtons (QMessageBox::No | QMessageBox::Yes);
        box.setDefaultButton   (QMessageBox::Yes);
        box.setButtonText(QMessageBox::Yes, "是");
        box.setButtonText(QMessageBox::No, "否");
        if (box.exec() == QMessageBox::Yes) {
            if (!openUrl().isEmpty())
                QDesktopServices::openUrl (QUrl (openUrl()));

            else if (downloaderEnabled()) {
                setFileName (downloadUrl().split ("/").last());
                startDownload (QUrl (downloadUrl()));
            }

            else
                QDesktopServices::openUrl (QUrl (downloadUrl()));
        }
    }

    else if (notifyOnFinish()) {
        box.setStandardButtons (QMessageBox::Close);
        box.setButtonText(QMessageBox::Close, "关闭");
           box.setInformativeText ("已经是最新版本");
        box.setText ("<h3>"
                     + QString("恭喜你，升级到了 %1 ").arg (moduleName())
                     + "</h3>");

        box.exec();
    }
}

/**
 * Compares the two version strings (\a x and \a y).
 *     - If \a x is greater than \y, this function returns \c true.
 *     - If \a y is greater than \x, this function returns \c false.
 *     - If both versions are the same, this function returns \c false.
 */
bool Updater::compare (const QString& x, const QString& y) {
    QStringList versionsX = x.split (".");
    QStringList versionsY = y.split (".");

    int count = qMin (versionsX.count(), versionsY.count());

    for (int i = 0; i < count; ++i) {
        int a = QString (versionsX.at (i)).toInt();
        int b = QString (versionsY.at (i)).toInt();

        if (a > b)
            return true;

        else if (b > a)
            return false;
    }

    return versionsY.count() < versionsX.count();
}

/**
 * Rounds the given \a input to two decimal places
 */
qreal Updater::round (const qreal& input) {
    return roundf (input * 100) / 100;
}

QString Updater::calculateTimeRemaining (qint64 received, qint64 total)
{
    uint difference = QDateTime::currentDateTime().toTime_t() - m_startTime;
    QString timeString;
    if (difference > 0) {
        qreal timeRemaining = total / (received / difference);

        if (timeRemaining > 7200) {
            timeRemaining /= 3600;
            int hours = int (timeRemaining + 0.5);

            if (hours > 1)
                timeString = QString("大约%1小时").arg (hours);
            else
                timeString = "大约1小时";
        }

        else if (timeRemaining > 60) {
            timeRemaining /= 60;
            int minutes = int (timeRemaining + 0.5);

            if (minutes > 1)
                timeString = QString("大约%1分钟").arg (minutes);
            else
                timeString = QString("大约1分钟");
        }

        else if (timeRemaining <= 60) {
            int seconds = int (timeRemaining + 0.5);

            if (seconds > 1)
                timeString = QString("%1秒").arg (seconds);
            else
                timeString = "1秒";
        }

    }

    return timeString;
}

void Updater::calculateSizes (qint64 received, qint64 total, QString& totalSize, QString& receivedSize) {
    if (total < 1024)
        totalSize = QString("%1 bytes").arg (total);

    else if (total < 1048576)
        totalSize = QString("%1 KB").arg (round (total / 1024));

    else
        totalSize = QString("%1 MB").arg (round (total / 1048576));

    if (received < 1024)
        receivedSize = QString("%1 bytes").arg (received);

    else if (received < 1048576)
        receivedSize = QString("%1 KB").arg (received / 1024);

    else
        receivedSize = QString("%1 MB").arg (received / 1048576);
}

void Updater::startDownload (const QUrl& url)
{
    /* Start download */
    m_startTime = QDateTime::currentDateTime().toTime_t();
    m_reply = m_manager->get (QNetworkRequest (url));
    connect (m_reply, SIGNAL (downloadProgress (qint64, qint64)),
                 this,      SLOT (updateProgress   (qint64, qint64)));
    connect (m_reply, SIGNAL (redirected       (QUrl)),
                 this,      SLOT (startDownload    (QUrl)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));

    /* Ensure that downloads directory exists */
    if (!DOWNLOAD_DIR.exists())
        DOWNLOAD_DIR.mkpath (".");

    /* Remove old downloads */
    QFile::remove (DOWNLOAD_DIR.filePath (m_filename));
    QFile::remove (DOWNLOAD_DIR.filePath (m_filename + PARTIAL_DOWN));
}

/**
 * Changes the name of the downloaded file
 */
void Updater::setFileName (const QString& file) {
    m_filename = file;

    if (m_filename.isEmpty())
        m_filename = "QSU_Update.bin";
}

void Updater::openDownload() {
    if (!m_filename.isEmpty())
        QDesktopServices::openUrl (QUrl::fromLocalFile (DOWNLOAD_DIR.filePath (
                                       m_filename)));

    else {
        QMessageBox msgBox;
        msgBox.critical (0,
                               "出错啦",
                               "找不到已经下载的更新!",
                               QMessageBox::Close);
        msgBox.setButtonText(QMessageBox::Close, "关闭");

    }
}

void Updater::installUpdate() {
    if (useCustomInstallProcedures())
        return;

    /* Ask the user to install the download */
    QMessageBox box;
    box.setIcon (QMessageBox::Question);
    box.setDefaultButton   (QMessageBox::Ok);

    box.setStandardButtons (QMessageBox::Ok | QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Ok, "确定");
    box.setButtonText(QMessageBox::Cancel, "取消");
    box.setInformativeText ("点击 \"确定\" 安装更新");
    box.setText ("<h3>要安装更新，请先退出程序.</h3>");

    /* User wants to install the download */
    if (box.exec() == QMessageBox::Ok) {
        if (!useCustomInstallProcedures())
            openDownload();
    }
}

void Updater::cancelDownload() {
    if (!m_reply->isFinished()) {
        QMessageBox box;
        box.setWindowTitle ("更新");
        box.setIcon (QMessageBox::Question);
        box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        box.setButtonText(QMessageBox::Yes, "是");
        box.setButtonText(QMessageBox::No, "否");
        box.setText ("确定取消下载吗?");

        if (box.exec() == QMessageBox::Yes) {
            m_reply->abort();
        }
    }
}

void Updater::saveFile (qint64 received, qint64 total) {
    /* Check if we need to redirect */
    QUrl url = m_reply->attribute (
                   QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        startDownload (url);
        return;
    }

    /* Save downloaded data to disk */
    QFile file (DOWNLOAD_DIR.filePath (m_filename + PARTIAL_DOWN));
    if (file.open (QIODevice::WriteOnly | QIODevice::Append)) {
        file.write (m_reply->readAll());
        file.close();
    }

    /* Open downloaded update */
    if (received >= total && total > 0) {
        /* Rename file */
        QFile::rename (DOWNLOAD_DIR.filePath (m_filename + PARTIAL_DOWN),
                       DOWNLOAD_DIR.filePath (m_filename));

        /* Notify application */
        emit downloadFinished (m_url, DOWNLOAD_DIR.filePath (m_filename));

        /* Install the update */
        m_reply->close();
        installUpdate();
    }
}

void Updater::updateProgress (qint64 received, qint64 total) {
    if (total > 0) {
        saveFile(received, total);
        QString receiveSize, totalSize;
        QString time;
        calculateSizes(received, total, totalSize, receiveSize);
        time = calculateTimeRemaining(received, total);
        emit downloadProgress(receiveSize, totalSize);
        emit timeProgress(time);
    }
}


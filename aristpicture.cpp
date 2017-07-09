#include "aristpicture.h"
#include "ui_aristpicture.h"
#include "getcirclescaledpixmap.h"
#include <QDebug>
#include <QKeyEvent>

AristPicture::AristPicture(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AristPicture)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::NoFrame);
    setPalette(parent->palette());
    ui->label->setPalette(parent->palette());
    ui->label->setForegroundRole(QPalette::Foreground);
    m_timer = new QTimer(this);
    m_timerPic = new QTimer(this);
    m_iCurrentRotateAngle = 0;
    m_mixedPic.load(":/img/cd.png");

    connect(ui->label, SIGNAL(enterSignal()), this, SLOT(enterSlot()));
    connect(ui->label, SIGNAL(leaveSignal()), this, SLOT(leaveSlot()));
    connect(ui->label, SIGNAL(clickSignal()), this, SLOT(clickSlot()));
    connect(m_timer, SIGNAL(timeout()), this,  SLOT(scrollInfo()));
    connect(m_timerPic, SIGNAL(timeout()), this, SLOT(showwhirlPicSlot()));
    m_timer->setInterval(250);
    m_timer->start();

    m_timerPic->setInterval(40);
    m_timerPic->start();

    m_iCurrentState = STATE_STOP;
    m_channelInfo = QStringLiteral("空");
}

AristPicture::~AristPicture()
{
    delete ui;
}

void AristPicture::scrollInfo()
{
    static int nPos = 0;
    QString scrollStr;

    scrollStr += QStringLiteral("频道: ");
    scrollStr += m_channelInfo;

    if (nPos > scrollStr.length())
    {
        nPos = 0;
    }


    QString info = QString("<p><span style=' font-weight:600; color:#aaaa00;'>");
    info += scrollStr.mid(nPos);
    info += QString("</span></p>");

    ui->label_info->setText(info);
    nPos++;
}

void AristPicture::channelChangedSlot(QString channel)
{
    m_channelInfo = channel;
}

void AristPicture::setDefultPic()
{
    m_iCurrentRotateAngle = 0;
    m_mixedPic.load(":/img/cd.png");
    ui->label->setPixmap(m_mixedPic);
    ui->label->show();
}

void AristPicture::setTotalTime(qint64 totalTime)
{
    m_totalTime = totalTime;
}

void AristPicture::enterSlot()
{
    if (m_iCurrentState != STATE_STOP)
    {
        m_timerPic->setInterval(1000);
    }
    if(m_iCurrentState == STATE_PALYING)
    {
        QPixmap pixmap;
        pixmap.load(":/img/pause.png");

        QPixmap pixMix;

        GetCircleScaledPixmap destPic;

        pixMix = destPic.getCirclePictureAtCenter(pixmap, m_mixedCurrentPic, 85, 85);//290, 290

        ui->label->setPixmap(pixMix);
        ui->label->show();

    } else if (m_iCurrentState == STATE_PAUSE)
    {
        QPixmap pixmap;
        pixmap.load(":/img/play.png");

        QPixmap pixMix;

        GetCircleScaledPixmap destPic;

        pixMix = destPic.getCirclePictureAtCenter(pixmap, m_mixedCurrentPic, 85, 85);//290, 290

        ui->label->setPixmap(pixMix);
        ui->label->show();
    }

}

void AristPicture::clickSlot()
{
    if(m_iCurrentState == STATE_PALYING)
    {
        m_iCurrentState = STATE_PAUSE;
        emit pauseSignal(true);


        QPixmap pixmap;
        pixmap.load(":/img/play.png");

        QPixmap pixMix;

        GetCircleScaledPixmap destPic;

        pixMix = destPic.getCirclePictureAtCenter(pixmap, m_mixedCurrentPic, 85, 85);//290, 290

        ui->label->setPixmap(pixMix);
        ui->label->show();
    }
    else if(m_iCurrentState == STATE_PAUSE)
    {
        m_iCurrentState = STATE_PALYING;
        emit pauseSignal(false);

        QPixmap pixmap;
        pixmap.load(":/img/pause.png");

        QPixmap pixMix;

        GetCircleScaledPixmap destPic;

        pixMix = destPic.getCirclePictureAtCenter(pixmap, m_mixedCurrentPic, 85, 85);//290, 290

        ui->label->setPixmap(pixMix);
        ui->label->show();
    }
}

void AristPicture::leaveSlot()
{
    m_timerPic->setInterval(40);
}

void AristPicture::setAristPic(QPixmap pixmap)
{
    QPixmap picBackGournd(":/img/cd.png");
    GetCircleScaledPixmap destPic;
    m_aristPic = pixmap;
    m_mixedCurrentPic = m_mixedPic = destPic.getCirclePictureAtCenter(pixmap, picBackGournd);//290, 290

    if(STATE_PALYING != m_iCurrentState)
    {
        m_iCurrentState = STATE_PALYING;
    }
}

void AristPicture::showwhirlPicSlot()
{
    if (m_totalTime <= 0 || m_iCurrentState != STATE_PALYING) return;
    m_iCurrentRotateAngle += 1;

    QTransform trs;
    trs.rotate(m_iCurrentRotateAngle);

    QPixmap pix;
    pix = m_mixedPic.transformed(trs);

    qreal fx = 0;
    qreal fy = 0;

    fx = (pix.width() - m_mixedPic.width())/2.0;
    fy = (pix.height() - m_mixedPic.height())/2.0;
    pix = pix.copy(fx, fy, m_mixedPic.width(), m_mixedPic.height());

    m_mixedCurrentPic = pix = pix.scaled(QSize(width(), height()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label->setPixmap(pix);
    ui->label->show();
}

void AristPicture::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space)
    {
        clickSlot();
    }
}

void AristPicture::saveAristPic(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.exists() && !m_aristPic.isNull())
    {
        m_aristPic.toImage().save(fileName);
        file.close();
    }
}

QPixmap AristPicture::loadAristPic(QString fileName)
{
    QPixmap aristPic;
    aristPic.load(fileName);

    return aristPic;
}

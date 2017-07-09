#include "getcirclescaledpixmap.h"
#include <QDebug>

GetCircleScaledPixmap::GetCircleScaledPixmap(QObject *parent) : QObject(parent)
{

}


QPixmap GetCircleScaledPixmap::getCirclePictureAtCenter(QPixmap srcPic, QPixmap backPic, int width, int height)
{
    int iCenterX = 0;
    int iCenterY = 0;
    iCenterX =  (backPic.width() - width) / 2.0;
    iCenterY = (backPic.height() - height) / 2.0;

    //在backPic上画源图片
    QPainter paint(&backPic);
    QRegion region(iCenterX, iCenterY, width, height, QRegion::Ellipse);
    paint.setClipRegion(region);

    paint.drawPixmap(iCenterX, iCenterY, width, height, srcPic);

    return backPic;
}


QPixmap GetCircleScaledPixmap::getCirclePictureAtCenter(QPixmap srcPic, QPixmap backPic)
{
    int iCenterX = 0;
    int iCenterY = 0;
    int width = 0;
    int height = 0;

    width = srcPic.width();
    height = srcPic.height();

    //在backPic上画源图片
    QPainter paint(&backPic);
    QRegion region(iCenterX, iCenterY, width, height, QRegion::Ellipse);
    paint.setClipRegion(region);

    paint.drawPixmap(iCenterX, iCenterY, width, height, srcPic);

    return backPic;
}

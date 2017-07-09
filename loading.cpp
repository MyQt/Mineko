#include "loading.h"
#include "ui_loading.h"
#include <QDesktopWidget>
#include <QMovie>

loading::loading(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::loading)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::NoFrame);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//无边框,置顶
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setStyleSheet("background-color:transparent");
    //屏幕居中显示
    int deskWidth = parent->width();
    int deskHeight = parent->height();
    setGeometry(0, 0, deskWidth, deskHeight);
    ui->labelWait->setGeometry(0, 0, deskWidth, deskHeight);
    //加载gif图片
    QMovie *movie = new QMovie(":/img/loading.gif");
    movie->setScaledSize(QSize(ui->labelWait->width(), ui->labelWait->height()));
    ui->labelWait->setMovie(movie);


    movie->start();
}

loading::~loading()
{
    delete ui;
}

#include"playercontrols.h"
#include"player.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include<QDebug>
#include<QMediaPlayer>
VideoWidget::VideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);//水平   垂直

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);//设置初始视频播放窗口为黑色
    setPalette(p);
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}



void VideoWidget::mousePressEvent(QMouseEvent *event)
{

    emit pressSignal();
    event->accept();
}

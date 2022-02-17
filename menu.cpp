#include "menu.h"
#include "ui_menu.h"
#include<QDebug>
menu::menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menu)
{
    ui->setupUi(this);

    video=new Player();
    image=new ImageViewer();
    text= new TextEdit();

    connect(video,&Player::mySignal,this,&menu::videoSub);//建立连接，player窗口点击退出，发送信号，调用videoSub（）显示菜单

    connect(image,&ImageViewer::mySignal,this,&menu::imageSub);//建立连接，imageviewer窗口点击退出，发送信号，调用imageSub（）显示菜单

    connect(text,&TextEdit::mySignal,this,&menu::textSub);//建立连接，player窗口点击退出，发送信号，调用Sub（）显示菜单

}

menu::~menu()
{
    delete ui;
}

void menu::videoSub()
{

    this->show();

}

void menu::imageSub()
{

    this->show();

}

void menu::textSub()
{

    this->show();

}

void menu::on_videoButton_clicked()//音视频播放器按钮
{
    this->hide();
    video->show();
}

void menu::on_pushButton_clicked()//图片浏览器按钮
{
    this->hide();
    image->show();
}

void menu::on_pushButton_2_clicked()//富文本编辑器按钮
{
    this->hide();
    text->show();
}

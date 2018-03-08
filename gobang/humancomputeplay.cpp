#include "humancomputeplay.h"

humanComputePlay::humanComputePlay(QWidget *parent) : Board(parent)
{
    setButtonLabel();
}

void humanComputePlay::setButtonLabel()
{
    //显示悔棋按钮
    this->pullBack = new QPushButton(this);
    this->pullBack->setText(QString("悔棋"));
    this->pullBack->setMaximumSize(80,40);
    this->pullBack->setMinimumSize(80,40);
    this->pullBack->move(200,480);
    connect(pullBack,&QPushButton::clicked,this,&Board::clickedPB);

    //显示自己的昵称头像和对方的昵称头像
    gname = new QLabel(this);
    gpicture = new QLabel(this);
    gtime = new QLabel(this);
    rtime = new QLabel(this);
    rpicture = new QLabel(this);
    rname = new QLabel(this);
    this->setNameg(QString("昵称：") + oneGame.myName);
    this->setPictureg(QString("kk"));
    this->setTimeg(QString("time: 1:30"));
    this->setNamer(QString("昵称：") + QString("电脑"));
    this->setPicturer(QString("asd"));
    this->setTimer(QString("time: 1:30"));
    update();//重新绘制
}


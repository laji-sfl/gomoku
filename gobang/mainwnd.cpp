#include "mainwnd.h"
#include "humancomputeplay.h"
#include "board.h"
#include "networkplay.h"
#include <QHBoxLayout>
#include <QDebug>

MainWnd::MainWnd(char num,QString name, QWidget *parent) : QWidget(parent)
{
    if(num == '1')
    {
//      Board *LocalBattleTwoPlayer = new Board(this);
      humanComputePlay *hum = new humanComputePlay(this);
      //把名字传入
      hum->oneGame.myName = name;
      hum->setNameg(QString("昵称：") + name);
    }
    else if(num == '2')
    {
        NetWorkPlay *net = new NetWorkPlay(this);
        net->oneGame.myName = name;
//        qDebug() << name << "name";
        net->setNameg(QString("昵称：") + name);
    }
//    QHBoxLayout* hLay = new QHBoxLayout(this);
//    hLay->addWidget(LocalBattleTwoPlayer, 1);
}


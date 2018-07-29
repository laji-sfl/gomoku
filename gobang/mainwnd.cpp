#include "mainwnd.h"
#include "humancomputeplay.h"
#include "board.h"
#include "networkplay.h"
#include <QHBoxLayout>
#include <QDebug>

MainWnd::MainWnd(char num,QString name, char *aesKey, QWidget *parent) : QWidget(parent)
{
    setMaximumSize(660, 530);

    if(num == '1')
    {
      humanComputePlay *hum = new humanComputePlay(this);
      hum->oneGame.myName = name;
      hum->setNameg(QString("昵称：") + name);
    }
    else if(num == '2')
    {
        NetWorkPlay *net = new NetWorkPlay(this);
        net->oneGame.myName = name;
        net->oneGame.aes_key = aesKey;
        net->setNameg(QString("昵称：") + name);
    }
}

MainWnd::~MainWnd()
{

}


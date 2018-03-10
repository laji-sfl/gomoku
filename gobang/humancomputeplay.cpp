#include "humancomputeplay.h"
#include <QMouseEvent>
#include <QDebug>
#include <QTime>
#include  <QtGlobal>
#include <QMessageBox>

humanComputePlay::humanComputePlay(QWidget *parent) : Board(parent)
{
    flagWho = true;
    flagStart = false;
    setButtonLabel();
    connect(this,SIGNAL(sendCompute()),this,SLOT(recvCompute()));
}

//设置悔棋按钮和标签
void humanComputePlay::setButtonLabel()
{
    //显示悔棋按钮
    this->pullBack = new QPushButton(this);
    this->pullBack->setText(QString("悔棋"));
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
    this->setTimeg(QString("time: 0:00"));
    this->setNamer(QString("昵称：") + QString("电脑"));
    this->setPicturer(QString("asd"));
    this->setTimer(QString("time: 0:00"));
    update();//重新绘制
}

//悔棋槽函数
void humanComputePlay::clickedPB()
{
    if(oneGame.stepList->length() < 2)
        return;

    auto tmp = oneGame.stepList->at(oneGame.stepList->length() - 1);
    auto tmp1 = oneGame.stepList->at(oneGame.stepList->length() - 2);

    if(tmp->gr == '2')  //假设点击悔棋的时候机器已经走了
    {
        oneGame.stonePos[tmp->x][tmp->y] = '0';
        oneGame.stonePos[tmp1->x][tmp1->y] = '0';
        oneGame.stepList->removeLast();
        oneGame.stepList->removeLast();

        flagWho = true;
        gameTime = 90;  //重新计时
        setTimer(QString("time: 1:30"));
        update();
    }
}

//开始按钮
void humanComputePlay::startTimerGame()
{
    if(flagStart)
        return;

    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            oneGame.stonePos[i][j] = '0';
        }
    }
    update();
    flagStart = true;

    Board::startTimerGame();
    this->gr = '1';     //玩家是1电脑是2
    flagWho = true;     //true是玩家下
}

//鼠标点击
void humanComputePlay::mouseReleaseEvent(QMouseEvent *ev)
{
    if(!flagWho)
        return;
    if(ev->button() != Qt::LeftButton)
        return;
    if(!flagStart)
        return;

    int x = -1, y = -1;

    computePos(ev->pos(), x, y);

    setTimer(QString("time:1:30"));
    this->gameTime = 90;
    if(!setGameDataPosStep(x, y, gr))   // forever gr == '1'
        return; //无效的点击不让电脑知道

    flagWho = false;

    update();   //重新绘制
    if(judgeWin())
    {
        gameOver();
        return; //不在往下执行
    }

    //发射信号
    emit sendCompute();
}

void humanComputePlay::recvCompute()
{
    computeMove();
}

void humanComputePlay::gameOver()
{
    delete this->timer;
    setTimeg(QString("time: 0:00"));
    setTimer(QString("time: 0:00"));
    flagStart = false;

    //清空链表，棋盘等到再一次开始的时候清除
    for(auto tmp = oneGame.stepList->begin();oneGame.stepList->end() != tmp; ++tmp)
    {
        delete (*tmp);  //释放 step*
    }
    qDebug() << "len:" << oneGame.stepList->length();
    oneGame.stepList->clear();

    QMessageBox::warning(this,"gameOver","gameOver laji!!!",QMessageBox::Yes,QMessageBox::Yes);
}

//机器走子
void humanComputePlay::computeMove()
{
    //计算出最佳的坐标
    int x = 0, y = 0;
    QTime t = QTime::currentTime();
    qsrand(t.second() * 1000 + t.msec());

    //test
    while(oneGame.stonePos[x][y] != '0')
    {
        x = qrand() % 14;
        y = qrand() % 14;
    }

    //更换玩家
    setTimeg(QString("time:1:30"));
    this->gameTime = 90;
    setGameDataPosStep(x, y, '2');  //电脑总是2
    flagWho = true;
    update();   //重新绘制

    if(judgeWin())
    {
        gameOver();
        return;
    }
}

void humanComputePlay::dealTime0()
{
    //人机，玩的就是霸王条款，超就超了
}

#include "humancomputeplay.h"
#include <QMouseEvent>
#include <QDebug>
#include <QTime>
#include  <QtGlobal>
#include <QMessageBox>

humanComputePlay::humanComputePlay(QWidget *parent) : Board(parent)
{
    this->autoBrain = new Brain;

    this->degreeNum = new QLineEdit(this);
    this->degreeNum->move(90,0);
    this->degreeNum->setMaximumWidth(30);
    this->changeDegree = new QPushButton("修改难度", this);
    this->changeDegree->move(120, 0);
    connect(this->changeDegree, SIGNAL(clicked(bool)), this, SLOT(changDegree()));

    flagWho = true;
    flagStart = false;
    this->setButtonLabel();
    //this->setNameg(QString("昵称：最帅的我"));//没有昵称？？？？
    this->setNamer(QString("昵称：笨电脑"));
    this->setTimer(QString("time: 0:00"));
    this->setTimeg(QString("time: 0:00"));
    update();
    connect(this,SIGNAL(sendCompute()),this,SLOT(recvCompute()));
}

humanComputePlay::~humanComputePlay()
{
    delete autoBrain;
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

    this->gname->setText(QString("昵称:最帅的我"));
    update();
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

void humanComputePlay::changDegree()
{
    if (flagStart)
        return;
    QString num = this->degreeNum->text();
    this->autoBrain->setDegree(num.toInt());
    this->degreeNum->clear();
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
    oneGame.stepList->clear();

    QMessageBox::warning(this,"gameOver","gameOver laji!!!",QMessageBox::Yes,QMessageBox::Yes);
}

//机器走子
void humanComputePlay::computeMove()
{
    //计算出最佳的坐标
    int x = oneGame.stepList->at(oneGame.stepList->length() - 1)->x;
    int y = oneGame.stepList->at(oneGame.stepList->length() - 1)->y;
    this->autoBrain->autoMove(x, y);

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

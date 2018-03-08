#include "board.h"
#include <QPainter> //用于绘制东西
#include <QPen>     //控制画出来的颜色粗细等
#include <QMouseEvent>
#include <QPicture>
#include <QColor>
#include <QBrush>
#include <QDebug>   //测试数据
#include "gamedata.h"

Board::Board(QWidget *parent) : QFrame(parent)
{
    setWindowTitle("五子棋");

    //设置窗体的固定大小
    setMaximumSize(660, 530);
    setMinimumSize(660, 530);

    //初始化棋盘数据
    startPoint = 50;
    intvale = 30;
    gr = '3';      //没有点击开始游戏，不能点击棋盘,char不能是-1
    gameTime = 90;

    //初始化游戏数据步骤链表
    oneGame.stepList = new QList<step*>;

    /*
     * 父类构造函数执行了，和子类构造函数冲突了怎么办
     * 是设计的不对吗？将父类指针传入是否就可以实现多态
     */
    //设置按钮和label
//    setButtonLabel();
    setStartButton();

    //自定义的信号和槽，时间为0
    connect(this,SIGNAL(timeEqual0()),this,SLOT(dealTime0()));
}

Board::~Board()
{
    //将步骤链表中的结构体释放
    for(auto tmp = oneGame.stepList->begin();oneGame.stepList->end() != tmp; ++tmp)
    {
        delete *tmp;
    }
    delete oneGame.stepList;
}

//定时器
void Board::getTimer()
{
    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(setTimeLabel()));
    this->timer->start(1000); //每一秒发射一次
}

//时间等于0的槽函数
void Board::dealTime0()
{
//    qDebug() << "time over ";
    //暂时父类不做处理
}

//定时器槽函数
void Board::setTimeLabel()
{
    //暂时先固定时间90秒
    int minute = 0, second = 0;
    minute = gameTime / 60;
    if(minute == 1)
        second = gameTime - 60;
    else
        second = gameTime;

    //发射信号
    if(gameTime == 0)
        emit timeEqual0();

    --gameTime;
    QString str = QString("time:%1:%2").arg(minute).arg(second);

    //根据对手选择要设置的定时器
    if(gr == '1')
        setTimeg(str);
    else if(gr == '2')
        setTimer(str);
}

//设置开始按钮
void Board::setStartButton()
{
    this->startGame = new QPushButton(QString("开始"), this);
    this->startGame->setMaximumSize(80,40);
    this->startGame->setMinimumSize(80,40);
    this->startGame->move(300,480);
    connect(startGame,&QPushButton::clicked,this,&Board::startTimerGame);
}

//开始游戏的槽函数
void Board::startTimerGame()
{
    this->getTimer();
    this->gr = '1';
}

//设置悔棋和label
void Board::setButtonLabel()
{
    //显示悔棋按钮
    this->pullBack = new QPushButton(this);
    this->pullBack->setText(QString("悔棋"));
    this->pullBack->setMaximumSize(70,30);
    this->pullBack->setMinimumSize(70,30);
    this->pullBack->move(200,480);
    connect(pullBack,&QPushButton::clicked,this,&Board::clickedPB);

    //显示自己的昵称头像和对方的昵称头像
    gname = new QLabel(this);
    gpicture = new QLabel(this);
    gtime = new QLabel(this);
    rtime = new QLabel(this);
    rpicture = new QLabel(this);
    rname = new QLabel(this);
    this->setNameg(QString("la ji"));
    this->setPictureg(QString("kk"));
    this->setTimeg(QString("shi jian"));
    this->setNamer(QString("la ji"));
    this->setPicturer(QString("kk"));
    this->setTimer(QString("shi jian"));
    update();//重新绘制
//    qDebug() << "set hui qi and label";
}
//设置label
void Board::setNameg(QString str)
{
    this->gname->setText(str);
    this->gname->move(500, 145);    //这些数字位置都是根据实际情况调整的
}
void Board::setPictureg(QString )
{
    QPixmap pix(QString("/home/sfl/download/girl.jpeg"));
    pix = pix.scaled(90,90);
    this->gpicture->setPixmap(pix);
    this->gpicture->move(500,50);
}
void Board::setTimeg(QString str)
{
    this->gtime->setText(str);
    this->gtime->move(500,170);
    update();
}
void Board::setNamer(QString str)
{
    this->rname->setText(str);
    this->rname->move(500,425);
}
void Board::setPicturer(QString )
{
    QPixmap pix(QString("/home/sfl/download/girl.jpeg"));
    pix = pix.scaled(80,80);
    this->rpicture->setPixmap(pix);
    this->rpicture->move(500,340);
}
void Board::setTimer(QString str)
{
    this->rtime->setText(str);
    this->rtime->move(500,450);
    update();
}

//悔棋
void Board::clickedPB()
{
    //父类什么也不做
//    qDebug() << "hui qi";
}

//绘制棋盘
void Board::drawBoard(QPainter &p)
{
    p.save();   //保存当前画家的状态

    //反走样，用于消除锯齿，是边缘平滑等
    p.setRenderHint(QPainter::Antialiasing, true);
    //设置棋盘画笔
    p.setPen(QPen(QBrush(QColor(0, 0, 255)), 2));

    //横竖直线
    for(int i = 0;i < 15; ++i)
    {
        p.drawLine(startPoint + i * intvale, startPoint, startPoint + i * intvale, startPoint + 14 * intvale);
        p.drawLine(startPoint, startPoint + i * intvale, startPoint + 14 * intvale, startPoint + i * intvale);
    }

    //中间的几个小点
    p.setBrush(Qt::blue);
    p.drawEllipse(startPoint - 7 + intvale * 3, startPoint - 7 + intvale * 3, 14, 14);
    p.drawEllipse(startPoint - 7 + intvale * 11, startPoint - 7 + intvale * 3, 14, 14);
    p.drawEllipse(startPoint - 7 + intvale * 3, startPoint - 7 + intvale * 11, 14, 14);
    p.drawEllipse(startPoint - 7 + intvale * 11, startPoint - 7 + intvale * 11, 14, 14);
    p.drawEllipse(startPoint - 7 + intvale * 7, startPoint - 7 + intvale * 7, 14, 14);

    p.restore();//回复当前画家的状态
}

//绘制棋子
void Board::drawStone(QPainter &p, char data[15][15])
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);

    //根据数组，1绿色，2红色，其他不画
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(data[i][j] == '1')
            {
                p.setBrush(Qt::green);
                p.drawEllipse(startPoint - 10 + intvale * i, startPoint - 10 + intvale * j, 20, 20);
            }
            else if(data[i][j] == '2')
            {
                p.setBrush(Qt::red);
                p.drawEllipse(startPoint - 10 + intvale * i, startPoint - 10 + intvale * j, 20, 20);
            }
        }
    }

    p.restore();
}

//绘制事件
void Board::paintEvent(QPaintEvent *)
{
    QPainter per(this);

    //画棋盘
    this->drawBoard(per);

    //画棋子
    this->drawStone(per, oneGame.stonePos);
}

//将鼠标点击的位置转换为棋盘上的坐标
void Board::computePos(QPoint pos, int &x, int &y)
{
    if(pos.x() < 40 || pos.y() < 40 || pos.x() > (14 * intvale + 60) || pos.y() > (14 * intvale + 60))
        return;

    x = (pos.x() -  40) / 30;
    y = (pos.y() -  40) / 30;
}

//设置游戏的棋子的坐标和下棋的步骤
void Board::setGameDataPosStep(int x, int y, char who)
{
    if(x == -1 || y == -1)
        return;

    if(oneGame.stonePos[x][y] != '0')
        return;

    oneGame.stonePos[x][y] = who;//内存棋盘
    step *oneStep = new step;   //步骤链表
    oneStep->x = x;
    oneStep->y = y;
    oneStep->gr = who;
    oneGame.stepList->append(oneStep);
}

//鼠标事件
void Board::mouseReleaseEvent(QMouseEvent *ev)
{
    //不是左键点击不予理会
    if(ev->button() != Qt::LeftButton)
        return;
    //没开始游戏不能点棋盘
    if(this->gr == '3')
        return;

    int x = -1, y = -1;

    //计算出点击的坐标
    computePos(ev->pos(), x, y);

    if(gr == '1')   //简单的实现两个对手之间的切换
    {
        gr = '2';
        setTimeg(QString("time:1:30"));
        this->gameTime = 90;
        setGameDataPosStep(x, y, gr);
    }
    else if(gr == '2')
    {
        gr = '1';
        setTimer(QString("time:1:30"));
        this->gameTime = 90;
        setGameDataPosStep(x, y, gr);
    }
    update();   //重新绘制
    if(judgeWin())
        qDebug() << "game over";
}

//每次下过一步之后就检测一次是否有五个子相连的情况
/*
 * 暂时只做简单的判断，当写人机对战的类的时候在分析局势根据局势判断输赢
 */
bool Board::judgeWin()
{
    //检测每一条线上的数据，比沿着点去检测要简单
    int tmp = 1;    //记录连续的个数
    int x = 0,y = 0;//根据起始坐标变化的坐标
    int start = 0, end = 4;//遍历坐标的起始坐标

    //横线
    for(int i = 0;i < 15; ++i)  // 15是横线的个数
    {
        for(int j = 0;j < 14; ++j)// 14横线个数减一
        {
            if(oneGame.stonePos[i][j] != '0' && oneGame.stonePos[i][j] == oneGame.stonePos[i][j + 1])
            {
                ++tmp;
                if(tmp == 5) return true;
            }
            else
            {
                tmp = 1;
            }
        }
    }

    //竖线
    for(int j = 0;j < 15; ++j)
    {
        tmp = 1;
        for(int i = 0;i < 14; ++i)
        {
            if(oneGame.stonePos[i][j] != '0' && oneGame.stonePos[i][j] == oneGame.stonePos[i + 1][j])
            {
                ++tmp;
                if(tmp == 5) return true;
            }
            else
            {
                tmp = 1;
            }
        }
    }

    //斜线
    for(int i = 0;i < 21; ++i)
    {
        x = start; y = end;
        tmp = 1;
        while(start != y)
        {
            if(oneGame.stonePos[x][y] != '0' && oneGame.stonePos[x][y] == oneGame.stonePos[x + 1][y - 1])   // x y要变化
            {
                ++tmp;
                if(tmp == 5) return true;
            }
            else
                tmp = 1;
            //向右上方增长
            x++;
            y--;
        }

        if(end < 14)    //根据固定的线的数量改变起始坐标
            end++;
        else
            start++;
    }

    //反斜线
    start = 10; end = 0;
    for(int i = 0;i < 21; ++i)
    {
        x = start; y = end;
        tmp = 1;
        while((start + y) != 14)
        {
            if(oneGame.stonePos[x][y] != '0' && oneGame.stonePos[x][y] == oneGame.stonePos[x + 1][y + 1])   //之前忘记了加一
            {
                ++tmp;
                if(tmp == 5) return true;
            }
            else
                tmp = 1;

            x++;
            y++;
        }

        if(start > 0)
            start--;
        else
            end++;
    }

    return false;
}

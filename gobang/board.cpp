#include "board.h"
#include <QPainter> //用于绘制东西
#include <QPen>     //控制画出来的颜色粗细等
#include <QMouseEvent>
#include <QColor>
#include <QBrush>
#include <QDebug>   //测试数据
#include "gamedata.h"

Board::Board(QWidget *parent) : QFrame(parent)
{
    setWindowTitle("五子棋");

    //设置窗体的固定大小
    setMaximumSize(680, 520);
    setMinimumSize(680, 520);

    //初始化棋盘数据
    startPoint = 50;
    intvale = 30;
    gr = '1';

    //初始化游戏数据步骤链表
    oneGame.stepList = new QList<step*>;

     /* 测试
     */
    //oneGame.stonePos[0][0] = '1';
    //oneGame.stonePos[0][14] = '2';

    //显示计时器，倒计时

    //显示悔棋按钮

    //显示自己的昵称头像和对方的昵称头像
}

Board::~Board()
{
    //将步骤链表中的结构体释放
    for(auto tmp = oneGame.stepList->begin();oneGame.stepList->end() != tmp; ++tmp)
    {
//        qDebug() << "delete stepList";
        delete *tmp;
    }
    delete oneGame.stepList;
    qDebug() << "xi gou han shu";
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

//    qDebug() << "hui zhi yi ci";
}

//将鼠标点击的位置转换为棋盘上的坐标
void Board::computePos(QPoint pos, int &x, int &y)
{
    //qDebug() << pos.x() << "---" << pos.y();
    if(pos.x() < 40 || pos.y() < 40 || pos.x() > (14 * intvale + 60) || pos.y() > (14 * intvale + 60))
        return;

    x = (pos.x() -  40) / 30;
    y = (pos.y() -  40) / 30;

    //qDebug() << x << "+++" << y;
}

//设置游戏的棋子的坐标和下棋的步骤
void Board::setGameDataPosStep(int x, int y, char who)
{
    if(x == -1 || y == -1)
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

    int x = -1, y = -1;

    /*
     * 测试：点击显示棋子
     */
    computePos(ev->pos(), x, y);

    //输出x y
    qDebug() << x << "===" << y;

    if(gr == '1')   //简单的实现两个对手之间的切换
    {
        gr = '2';
        setGameDataPosStep(x, y, gr);
    }
    else
    {
        gr = '1';
        setGameDataPosStep(x, y, gr);
    }
    update();   //重新绘制
}

//每次下过一步之后就检测一次是否有五个子相连的情况
/*
 * 暂时只做简单的判断，当写人机对战的类的时候在分析局势根据局势判断输赢
 */
void Board::judgeWin()
{

}

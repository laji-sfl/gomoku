#include "board.h"
#include <QPainter> //用于绘制东西
#include <QPen>     //控制画出来的颜色粗细等
#include <QColor>
#include <QBrush>
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


     /* 测试
     */
    //oneGame.stonePos[0][0] = '1';
    //oneGame.stonePos[0][14] = '2';


}

Board::~Board()
{

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


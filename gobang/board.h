#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include <QWidget>
#include "gamedata.h"   //棋局的数据

/**
 * 这个类是所有游戏对战类的基类，也是一台电脑人人对战的类
 * 增加可以返回选择对战模式的功能。
 *   需要重写悔棋的槽函数来实现网络端的悔棋，
 *   重写定时器的函数，实现网络事件同步，
 * 人机对战类，重写鼠标点击事件，就可以实现电脑下棋
 * 网络对战类，重写鼠标点击事件，实现下棋位置的切换
 *   新添加聊天的函数
 *   新添加匹配对手的按钮，重新匹配
 *   将返回选择对战模式，重写为断开连接
 * 设计登录游戏注册游戏的类，直接就是网络通信的
 * 设计选择游戏对战模式的类，本地
 **/

class Board : public QFrame
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);    //构造函数
    ~Board();   //析构函数

    //绘制事件
    void paintEvent(QPaintEvent *);
    void drawBoard(QPainter &p);
    void drawStone(QPainter &p, char data[15][15]);

    //鼠标点击事件的处理
    void mouseReleaseEvent(QMouseEvent *ev);
    void computePos(QPoint pos, int &x, int &y);//计算出棋盘的坐标

    //设置游戏数据
    void setGameDataPosStep(int x, int y, char who);

    //判断输赢
    void judgeWin();

private:
    GameData oneGame;  //记录游戏数据
    int startPoint;    //起始的位置
    int intvale;       //间隔大小
    char gr;           //绿的还是红的
};

#endif // BOARD_H

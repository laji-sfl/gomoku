#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include <QWidget>
#include <QTimer>   //计时器
#include <QLabel>   //标签
#include <QPushButton>//按钮
#include "gamedata.h"   //棋局的数据

/**
 * 这个类是所有游戏对战类的基类，也是一台电脑人人对战的类
 * 增加可以返回选择对战模式的功能。
 *   需要重写悔棋的槽函数来实现网络端的悔棋，
 *   重写定时器的函数，实现网络时间同步，
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
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    void computePos(QPoint pos, int &x, int &y);//计算出棋盘的坐标

    //设置游戏数据
    virtual bool setGameDataPosStep(int x, int y, char who);

    //判断输赢
    virtual bool judgeWin();

    //设置label
    void setNameg(QString str); //昵称
    void setTimeg(QString str); //时间字符串
    void setPictureg(QString );  //图片的路径,暂时不支持切换图片，等后续版本
    void setNamer(QString str);
    void setTimer(QString str);
    void setPicturer(QString);

    //设置悔棋按钮和label
    virtual void setButtonLabel();
    void setStartButton();

    //设置定时器
    void getTimer();

//槽函数
public slots:
    //点击悔棋的槽函数
    virtual void clickedPB();
    //定时器时间到
    void setTimeLabel();
    //开始游戏
    virtual void startTimerGame();
    //当时间为0
    virtual void dealTime0();
    //重新选择
    void recvChoose();

//自定义信号
signals:
    void timeEqual0();

//子类需要用到，第一个版本就不麻烦了，先实现功能，在去优化代码
public:
    GameData oneGame;  //记录游戏数据
    int startPoint;    //棋盘起始的位置
    int intvale;       //棋盘间隔大小
    char gr;           //绿的还是红的,是否开始游戏
    QPushButton *pullBack;//后悔键
    QPushButton *startGame; //开始游戏
    QPushButton *reChoose;  //重新选择
    QLabel *gtime;   //计时器
    QLabel *gname;   //昵称
    QLabel *gpicture;//头像
    QLabel *rtime;
    QLabel *rname;
    QLabel *rpicture;
    QTimer *timer;  //计时器是类的属性，在子类中也可以捕捉时间信号
    int gameTime;   //每一个人的固定思考时间
};

#endif // BOARD_H

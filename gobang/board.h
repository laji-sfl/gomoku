#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include <QWidget>
#include "gamedata.h"   //棋局的数据

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

private:
    GameData oneGame;       //记录游戏数据
    int startPoint;    //起始的位置
    int intvale;       //间隔大小
};

#endif // BOARD_H

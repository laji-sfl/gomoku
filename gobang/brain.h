#ifndef BRAIN_H
#define BRAIN_H

#include <list>
#include <deque>
#include <iostream>

#define TWO0    200
#define TWO1    100
#define TWO2    20
#define THREE0  1000
#define THREE1  300
#define THREE2  30
#define FOUR0   10000
#define FOUR1   5000
#define FOUR2   40

struct Pos
{
    int x_axis; //棋子横坐标
    int y_axis; //纵坐标
    int value;  //棋局估值

    bool operator<(Pos &y)//用来对链表排序
    {
        return !(value < y.value);
    }
};

class Brain
{
public:
    Brain();
    ~Brain();

public:
    void autoMove(int &x, int &y);  //自己的算法

    inline void setDegree(int value)     //设置难度
    {
        if(value < 1 || value > 14) //非法难度就设置为1
            this->degree = 1;
        else
            this->degree = value;
    }

public:
    void canMovePos();
    void setPos(int x, int y);
    void clearThree();

    void setValue();

    int horizontalLine();
    int verticalLine();
    int slantedLine();
    int anticlinalLine();

    void getBestPos(int &x, int &y);
    int switchScore(int num, char flag);
    void setScore(int &score, int x, int y, std::deque<char> &pattern);
    void switchPattern(int &score_1, int &score_2, int x, int y, std::deque<char> &pattern);

//私有的数据结构
public:
    char chess[15][15];
    std::list<Pos> posSet;
    int degree;

};

#endif // BRAIN_H

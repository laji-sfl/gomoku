#ifndef AUTOBRAIN_H
#define AUTOBRAIN_H

#include <list>
#include <deque>
#include <iostream>

/*
 *  五子棋人工智能引擎，提供不同的智能计算方法。
 *
 *  一、总是玩家先走，棋盘根据难度系数来计算需要计算的数据量。
 *     遍历棋盘上的棋子，以每一个棋子为中心，向外寻找一到五个的位置，作为可走位置。
 *     简单的分析每一个子的功能，将一些无效的位置去除。//暂时没有实现，如何判断无用？
 *     分析剩下的子的棋局分数。
 *     将合适的棋子位置传出。
 *     尝试着添加多线程计算可以走的位置的数据，将链表进行拆分。也可以进行分布式计算。
 *
 *     暂时只实现了计算一步，计算多部的思路是根据难度预测走几步，机器得出最佳的位置之后，模拟人再来
 *        计算最佳的一步，在机器……，并且记录下棋的过程，如果在预测的几步之内人胜利或者分数变得很小
 *        就回退一部计算，知道没有什么特殊情况为止。
 *
 *  二、参考他人的博弈树算法、剪枝算法、还有与五子棋人工智能相关的算法。
 */

//估值的情况，暂时不管一个子的情况
//#define ONE0    30  // 表示几个子相连，两头有几个子堵着。
//#define ONE1    20
//#define ONE2    10
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

class AutoBrain
{
    Q_OBJECT
public:
    AutoBrain();
    ~AutoBrain();

//公共的函数接口
public:
    void autoMove(int &x, int &y);  //自己的算法
    //void smartMove(int &x, int &y); //参考的算法
    inline void setDegree(int value)     //设置难度
    {
        if(value < 1 || value > 14) //非法难度就设置为1
            this->degree = 1;
        else
            this->degree = value;
    }

//私有的自己用的函数
public:
    void canMovePos();  //计算此时棋盘上可以走的位置
    void setPos(int x, int y);//将可以移动的点记入链表
    void clearThree();  //将标记为3的落点变为0

    void setValue();    //设置链表每一个节点对应的棋盘分数

    int horizontalLine();//四个不同的线的分数。横线
    int verticalLine();//竖线
    int slantedLine();//斜线
    int anticlinalLine();//反斜线

    void getBestPos(int &x, int &y);//计算出最佳的一步，如果最佳的位置不止一个就随机，或者预测
    int switchScore(int num, char flag);//根据模式选择对应的分数
    void setScore(int &score, int x, int y, std::deque<char> &pattern);//设置链表节点的分数
    void switchPattern(int &score_1, int &score_2, int x, int y, std::deque<char> &pattern);//遍历一条线，选择线的模式

    //void computeDepth();//根据难度来进行深层次的预测运算，就是多想几步

//私有的数据结构
public:
    char chess[15][15]; //棋盘，‘0’无子，‘1’对手，‘2’自己,'3'标记为已经加入链表
    std::list<Pos> posSet;  //电脑可以走的位置的集合
                            //直接拷贝不会占太大内存，只多了2个int大小
    int degree; //难度系数从1开始
};

#endif // AUTOBRAIN_H

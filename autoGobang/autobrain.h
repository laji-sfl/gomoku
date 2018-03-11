#ifndef AUTOBRAIN_H
#define AUTOBRAIN_H

#include <list>

/*
 *  五子棋人工智能引擎，提供不同的智能计算方法。
 *
 *  一、主要是防守，因为总是玩家先走，棋盘根据难度系数来计算需要计算的数据量。
 *     遍历棋盘上的棋子，以每一个棋子为中心，向外寻找一到五个的位置，作为可走位置。
 *     简单的分析每一个子的功能，将一些无效的位置去除。
 *     分析剩下的子的棋局分数。
 *     将合适的棋子位置传出。
 *     尝试着添加多线程计算可以走的位置的数据，将链表进行拆分。
 *
 *  二、参考他人的博弈树算法、剪枝算法、还有与五子棋人工智能相关的算法。
 */

//估值的情况
#define ONE0    30  // 表示几个子相连，两头有几个子堵着。
#define ONE1    20
#define ONE2    10
#define TWO0    70
#define TWO1    40
#define TWO2    20
#define THREE0  80
#define THREE1  50
#define THREE2  30
#define FOUR0   100
#define FOUR1   90
#define FOUR2   30

struct Pos
{
    int x_axis;
    int y_axis;
    int value;  //棋局估值
};

class AutoBrain
{
public:
    AutoBrain();
    ~AutoBrain();

//公共的函数接口
public:
    void autoMove(int &x, int &y);  //自己的算法
    void smartMove(int &x, int &y); //参考的算法
    void setDegree(char value);     //设置难度

//私有的自己用的函数
private:
    void canMovePos(char degree);  //计算此时棋盘上可以走的位置
    void setValue();    //评估此时的棋盘分数，并写入链表
    void reducePos(char degree);   //根据难度来去除一些位置
    void computeDepth(char degree);//根据难度来进行深层次的预测运算，就是多想几步

//私有的数据结构
private:
    char chess[15][15]; //棋盘，‘0’无子，‘1’对手，‘2’自己
    std::list<Pos> posSet; //直接拷贝不会占太大内存，只多了2个int大小，暂时不考虑
    char degree; //难度系数从‘0‘开始，往后增大
};

#endif // AUTOBRAIN_H

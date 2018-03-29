#ifndef AUTOBRAIN_H
#define AUTOBRAIN_H

#include <stack>
#include <list>
#include <iostream>

/*
 *  五子棋人工智能引擎，提供不同的智能计算方法。
 *
 *  一、主要是防守，因为总是玩家先走，棋盘根据难度系数来计算需要计算的数据量。
 *     遍历棋盘上的棋子，以每一个棋子为中心，向外寻找一到五个的位置，作为可走位置。
 *     简单的分析每一个子的功能，将一些无效的位置去除。
 *     分析剩下的子的棋局分数。
 *     将合适的棋子位置传出。
 *     尝试着添加多线程计算可以走的位置的数据，将链表进行拆分。也可以进行分布式计算。
 *
 *  二、参考他人的博弈树算法、剪枝算法、还有与五子棋人工智能相关的算法。
 */

//估值的情况
#define ONE0    30  // 表示几个子相连，两头有几个子堵着。
#define ONE1    20
#define ONE2    10
#define TWO0    60
#define TWO1    40
#define TWO2    20
#define THREE0  80
#define THREE1  60
#define THREE2  30
#define FOUR0   100
#define FOUR1   80
#define FOUR2   30


struct Pos
{
    int x_axis; //棋子横坐标
    int y_axis; //纵坐标
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
    //void smartMove(int &x, int &y); //参考的算法
    inline void setDegree(char value)     //设置难度
    {
        this->degree = value;
    }
    /**
     ** 测试函数
     **/
    void printList()
    {
        for(auto tmp : this->posSet)
            std::cout << tmp.x_axis << " : " << tmp.y_axis << std::endl;
    }
    void setChess(int i,int j,char ch)
    {
        chess[i][j] = ch;
    }

//私有的自己用的函数
public:
    void canMovePos();  //计算此时棋盘上可以走的位置
    void setPos(int degr, int x, int y);//根据难度设置链表的点
    void clearThree();  //将标记为3的落点变为0
    //计算横向和纵向不超过边界的范围
    inline int getMin(int pos,int distance)
    {
        return (pos - distance) < 0 ? 0 : (pos - distance);
    }
    inline int getMax(int pos,int distance)
    {
        return (pos + distance) > 14 ? 14 : (pos + distance);
    }
    //根据模式来匹配分数
    inline int matchWeight(int weight[4][3], int num, int i, int j);
    inline int matchWeight2(int weight[4][3], int num, int i, int j);

    void checkPattern(std::stack<char> &pat,int i,int &j,int &score1,int &score2,int weight[4][3]);
    void setValue();    //评估此时的棋盘分数，并写入链表
    void reducePos();   //根据难度来去除一些位置
    void computeDepth();//根据难度来进行深层次的预测运算，就是多想几步

//私有的数据结构
public:
    char chess[15][15]; //棋盘，‘0’无子，‘1’对手，‘2’自己,'3'标记为已经加入链表
    std::list<Pos> posSet; //直接拷贝不会占太大内存，只多了2个int大小，暂时不考虑
    char degree; //难度系数从‘0‘开始，往后增大
};

#endif // AUTOBRAIN_H

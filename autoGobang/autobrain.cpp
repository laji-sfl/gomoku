#include "autobrain.h"

AutoBrain::AutoBrain()
{
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            chess[i][j] = '0';
        }
    }
}

AutoBrain::~AutoBrain()
{

}

//自己的算法
void AutoBrain::autoMove(int &x, int &y)
{

}

//参考的算法
void AutoBrain::smartMove(int &x, int &y)
{

}

//设置难度
void AutoBrain::setDegree(char value)
{

}

//计算此时棋盘上可以走的位置
void AutoBrain::canMovePos(char degree)
{

}

//评估此时的棋盘分数，并写入链表
void AutoBrain::setValue()
{

}

//根据难度来去除一些位置
void AutoBrain::reducePos(char degree)
{

}

//根据难度来进行深层次的预测运算，就是多想几步
void AutoBrain::computeDepth(char degree)
{

}


#include "autobrain.h"
#include <algorithm>

AutoBrain::AutoBrain() : degree('1')
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
    chess[x][y] = '1';
    canMovePos();
    setValue();
    Pos max = *posSet.begin();
    for(auto tmp : posSet)
    {
        if(tmp.value > max.value)
            max = tmp;
    }
    x = max.x_axis;
    y = max.y_axis;
}

//参考的算法
//void AutoBrain::smartMove(int &x, int &y)
//{
//
//}

//计算此时棋盘上可以走的位置
void AutoBrain::canMovePos()
{
    int extendNum = degree - '0';//向外扩展的个数

    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(this->chess[i][j] == '1' || this->chess[i][j] == '2')
                this->setPos(extendNum,i,j);//将这个点周围的点写入链表
        }
    }

    //清空标志位
    this->clearThree();
}

//根据难度设置链表的点
void AutoBrain::setPos(int degr, int x, int y)
{
    //横线
    for(int i = getMin(x,degr);i <= getMax(x,degr); ++i)//向左向右扩展degr个子
    {
        if(chess[i][y] == '0')
        {
            chess[i][y] = '3';
            Pos tmp = {i,y,0};
            posSet.push_back(tmp);
        }
    }
    //竖线
    for(int i = getMin(y,degr);i <= getMax(y,degr); ++i)
    {
        if(chess[x][i] == '0')
        {
            chess[x][i] = '3';
            Pos tmp = {x,i,0};
            posSet.push_back(tmp);
        }
    }
    //斜线
    int beginX = x - degr,beginY = y - degr;    //起始的坐标

    for(int i = 0;i <= 2 * degr; ++i)   //共循环两倍的难度的个数
    {
        if(beginX >= 0 || beginY >= 0 || beginX <= 14 || beginY <= 14)
        {
            if(chess[beginX][beginY] == '0')
            {
                chess[beginX][beginY] = '3';
                Pos tmp = {beginX,beginY,0};
                posSet.push_back(tmp);
            }
        }
        ++beginX;   //不管怎样每次循环位置都要变
        ++beginY;
    }

    //反斜线
    beginX = x - degr;
    beginY = y + degr;    //起始的坐标
    for(int i = 0;i <= 2 * degr; ++i)
    {
        if(beginX >= 0 || beginY >= 0 || beginX <= 14 || beginY <= 14)
        {
            if(chess[beginX][beginY] == '0')
            {
                chess[beginX][beginY] = '3';
                Pos tmp = {beginX,beginY,0};
                posSet.push_back(tmp);
            }
        }
        ++beginX;   //不管怎样每次循环位置都要变
        --beginY;
    }
}

//清空标志位
void AutoBrain::clearThree()
{
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(chess[i][j] == '3')
                chess[i][j] = '0';
        }
    }
}

inline int AutoBrain::matchWeight(int weight[4][3], int num, int i, int j)
{
    if(chess[i][j] == '0')//x220
    {
        if((j - num - 1) >= 0)
        {
            if(chess[i][j - num - 1] == '0')//0220
            {
                return weight[num - 1][0];
            }
            else//1220
            {
                return weight[num - 1][1];
            }
        }
        else
            return weight[num - 1][1];  //到边界上相当于有一个棋子
    }
    else//x221
    {
        if((j - num - 1) >= 0)
        {
            if(chess[i][j - num - 1] == '0')//0221
            {
                return weight[num - 1][1];
            }
            else//1221
            {
                return weight[num - 1][2];
            }
        }
        else
            return weight[num - 1][2];
    }
}
inline int AutoBrain::matchWeight2(int weight[4][3], int num, int i, int j)
{
    if(chess[i][j] == '0')//x220
    {
        if((i - num - 1) >= 0)
        {
            if(chess[i-num-1][j] == '0')//0220
            {
                return weight[num - 1][0];
            }
            else//1220
            {
                return weight[num - 1][1];
            }
        }
        else
            return weight[num - 1][1];  //到边界上相当于有一个棋子
    }
    else//x221
    {
        if((i - num - 1) >= 0)
        {
            if(chess[i-num-1][j] == '0')//0221
            {
                return weight[num - 1][1];
            }
            else//1221
            {
                return weight[num - 1][2];
            }
        }
        else
            return weight[num - 1][2];
    }
}
void AutoBrain::checkPattern(std::stack<char> &pat, int i, int &j, int &score1, int &score2, int weight[4][3])
{
    if(pat.empty())
        pat.push(chess[i][j]);
    else
    {
        if(pat.top() == chess[i][j])
        {
            pat.push(chess[i][j]);
            if(j == 14)
            {
                if(pat.top() == '2')    //连续的是2
                {
                    score2 += weight[pat.size()][1];
                    while(!pat.empty()) pat.pop();
                }
                else if(pat.top() == '1')//连续的是1
                {
                    score1 += weight[pat.size()][1];
                    while(!pat.empty()) pat.pop();
                }
            }
        }
        else
        {
            if(pat.top() == '2')    //连续的是2
            {
                score2 += matchWeight(weight,pat.size(),i,j);
                while(!pat.empty()) pat.pop();
            }
            else if(pat.top() == '1')//连续的是1
            {
                score1 += matchWeight(weight,pat.size(),i,j);
                while(!pat.empty()) pat.pop();
            }
            j--;
            while(!pat.empty())pat.pop();
        }
    }
}

//评估此时的棋盘分数，并写入链表
void AutoBrain::setValue()
{
    //遍历链表中的每一个节点，按照节点的坐标给程序赋值‘2’
    //计算此时的局面分数，并写入链表
    //计算方法：遍历棋盘的每一条线，寻找宏中描绘的情况，敌人的是负值，我方是正值
    //把我方的值相加与敌人的相减，把得到的值复制给结构体
    //如何遍历：对于任意一条线，如果值相等就累加计数器，直到不相等，并判断计数器停止的
    //位置的值是棋子还是空的由此进行赋值
    int weight[4][3] = {ONE0,ONE1,ONE2,TWO0,TWO1,TWO2,THREE0,THREE1,THREE2,FOUR0,FOUR1,FOUR2};

    for(auto &tmp : this->posSet)
    {
        chess[tmp.x_axis][tmp.y_axis] = '2';//假定下这里
        int score1 = 0, score2 = 0; //初始的棋盘分数
        std::stack<char> pat;

        //横线,可以正确的计算出来,不好写成函数
        for(int i = 0;i < 15; ++i)  //横线的个数
        {
            for(int j = 0;j < 15; ++j)//横线上的点
            {
                checkPattern(pat,i,j,score1,score2,weight);
            }
        }

        //竖线
        while(!pat.empty())pat.pop();
        for(int j = 0;j < 15; ++j)
        {
            for(int i = 0;i < 15; ++i)
            {
                if(pat.empty())
                    pat.push(chess[i][j]);
                else
                {
                    if(pat.top() == chess[i][j])
                    {
                        pat.push(chess[i][j]);
                        if(i == 14)
                        {
                            if(pat.top() == '2')    //连续的是2
                            {
                                score2 += weight[pat.size()][1];
                                while(!pat.empty()) pat.pop();
                            }
                            else if(pat.top() == '1')//连续的是1
                            {
                                score1 += weight[pat.size()][1];
                                while(!pat.empty()) pat.pop();
                            }
                        }
                    }
                    else
                    {
                        if(pat.top() == '2')    //连续的是2
                        {
                            score2 += matchWeight2(weight,pat.size(),i,j);
                            while(!pat.empty()) pat.pop();
                        }
                        else if(pat.top() == '1')//连续的是1
                        {
                            score1 += matchWeight2(weight,pat.size(),i,j);
                            while(!pat.empty()) pat.pop();
                        }
                        i--;
                        while(!pat.empty())pat.pop();
                    }
                }
            }
        }

        //斜线
        while(!pat.empty())pat.pop();
        int x = 0,y = 0;
        int start = 0,end = 4;
        for(int i = 0;i < 21; ++i)
        {
            x = start;y = end;
            while(start != y)
            {
                if(pat.empty())
                    pat.push(chess[x][y]);
                else
                {
                    if(pat.top() == chess[x][y])
                    {
                        pat.push(chess[x][y]);//没有对边界进行判断
                    }
                    else
                    {
                        if(pat.top() == '2')    //连续的是2
                        {
                            score2 += matchWeight(weight,pat.size(),x,y);
                            while(!pat.empty()) pat.pop();
                        }
                        else if(pat.top() == '1')//连续的是1
                        {
                            score1 += matchWeight(weight,pat.size(),x,y);
                            while(!pat.empty()) pat.pop();
                        }
                        y++;x--;
                        while(!pat.empty())pat.pop();
                    }
                }

                x++;
                y--;
            }
            if(end < 14)
                end++;
            else
                start++;
        }

        //反斜线
        while(!pat.empty())pat.pop();
        start = 10;end = 0;
        for(int i = 0;i < 21; ++i)
        {
            x = start;y = end;
            while((start + y) != 14)
            {
                if(pat.empty())
                    pat.push(chess[x][y]);
                else
                {
                    if(pat.top() == chess[x][y])
                    {
                        pat.push(chess[x][y]);//没有对边界进行判断
                    }
                    else
                    {
                        if(pat.top() == '2')    //连续的是2
                        {
                            score2 += matchWeight(weight,pat.size(),x,y);
                            while(!pat.empty()) pat.pop();
                        }
                        else if(pat.top() == '1')//连续的是1
                        {
                            score1 += matchWeight(weight,pat.size(),x,y);
                            while(!pat.empty()) pat.pop();
                        }
                        y--;x--;
                        while(!pat.empty())pat.pop();
                    }
                }

                x++;
                y++;
            }
            if(start > 0)
                start--;
            else
                end++;
        }

        chess[tmp.x_axis][tmp.y_axis] = '0';//取消假定
//        std::cout << score1 << "dd" << score2 << std::endl;
        tmp.value = (score2 - score1);
    }
}

//根据难度来去除一些位置
void AutoBrain::reducePos()
{
    //暂时没有办法应对，下的很远的棋子，那需要在高难度中多部预测
    //简单的情况下，不进行预测步骤，没必要进行剪枝
}

//根据难度来进行深层次的预测运算，就是多想几步
void AutoBrain::computeDepth()
{

}


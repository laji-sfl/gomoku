#include "autobrain.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <ctime>

AutoBrain::AutoBrain() : degree(1)//难度初始化为1
{
    //初始化棋盘
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
//传入敌人的下棋位置，传出电脑的下棋位置
void AutoBrain::autoMove(int &x, int &y)
{
    chess[x][y] = '1';  //对方走的步骤
    canMovePos();       //所有电脑可以走的位置
    setValue();         //计算电脑可以走的位置的权值

    getBestPos(x, y);   //计算出最佳的位置，并返回
    chess[x][y] = '2';
}

//sort函数内部提示没有匹配减号，可能是因为全局的sort没有对list迭代器进行重载-
//bool sort_comp(std::list<Pos>::iterator x, std::list<Pos>::iterator y)
//{
//    return x->value < y->value;
//}

void AutoBrain::getBestPos(int &x, int &y)
{
    posSet.sort();

    //在所有相等的位置随机产生一个
//    auto p_equal_num = posSet.begin();
//    for(auto tmp = ++posSet.begin(); tmp != posSet.end(); ++tmp)
//    {
//        if((*p_equal_num).value == (*tmp).value)
//            p_equal_num++;
//        else break;
//    }

//    srand(time(NULL));
//    int ptr_len = 0;
//    if((ptr_len = distance(posSet.begin(), p_equal_num)) <= 0)
//        x = (*p_equal_num).x_axis, y = (*p_equal_num).y_axis;
//    else
//        ptr_len = rand() % (ptr_len - 1);

    auto p_equal_num = posSet.begin();
//    std::advance(p_equal_num, ptr_len);
    x = p_equal_num->x_axis;
    y = p_equal_num->y_axis;
}

//参考的算法
//void AutoBrain::smartMove(int &x, int &y)
//{
//
//}

//计算此时棋盘上可以走的位置
void AutoBrain::canMovePos()
{
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(this->chess[i][j] == '1' || this->chess[i][j] == '2')
                this->setPos(i, j);//将这个点周围的点写入链表
        }
    }

    //清空标志位
    this->clearThree();
}

//根据难度设置链表的点
void AutoBrain::setPos(int x, int y)
{
    //竖线
    for(int i = 1;i <= degree; ++i)
    {
        //如果棋盘上是空的，并且这个子没有超出棋盘范围，那么就可以走
        if(chess[x-i][y] == '0' && x-i >= 0)
        {
            Pos tmp{x-i,y,0};
            posSet.push_back(tmp);
            chess[x-i][y] = '3';
        }
        if (chess[x+i][y] == '0' && x+i <= 14)
        {
            Pos tmp2{x+i,y,0};
            posSet.push_back(tmp2);
            chess[x+i][y] = '3';
        }
    }

    //横线
    for(int i = 1;i <= degree; ++i)
    {
        if(chess[x][y-i] == '0' && y-i >= 0)
        {
            Pos tmp{x,y-i,0};
            posSet.push_back(tmp);
            chess[x][y-i] = '3';
        }
        if(chess[x][y+i] == '0' && y+i <= 14)
        {
            Pos tmp2{x,y+i,0};
            posSet.push_back(tmp2);
            chess[x][y+i] = '3';
        }
    }

    //斜线
    for(int i = 1;i <= degree; ++i)
    {
        if(chess[x-i][y+i] == '0' && x-i >= 0 && y+i <= 14)
        {
            Pos tmp{x-i,y+i,0};
            posSet.push_back(tmp);
            chess[x-i][y+i] = '3';
        }
        if(chess[x+i][y-i] == '0' && x+i <= 14 && y-i >= 0)
        {
            Pos tmp2{x+i,y-i,0};
            posSet.push_back(tmp2);
            chess[x+i][y-i] = '3';
        }
    }

    //反斜线
    for(int i = 1;i <= degree; ++i)
    {
        if(chess[x-i][y-i] == '0' && x-i >= 0 && y-i >= 0)
        {
            Pos tmp{x-i,y-i,0};
            posSet.push_back(tmp);
            chess[x-i][y-i] = '3';
        }
        if(chess[x+i][y+i] == '0' && x+i <= 14 && y+i <= 14)
        {
            Pos tmp2{x+i,y+i,0};
            posSet.push_back(tmp2);
            chess[x+i][y+i] = '3';
        }
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

//评估此时的棋盘分数，并写入链表
void AutoBrain::setValue()
{
    //遍历链表中的每一个节点，假定电脑走到了这里
    for(auto &tmp : posSet)
    {
        chess[tmp.x_axis][tmp.y_axis] = '2';    //假设电脑走到这里
        //横线
        tmp.value += horizontalLine();
        //竖线
        tmp.value += verticalLine();
        //斜线
        tmp.value += slantedLine();
        //反斜线
        tmp.value += anticlinalLine();
        chess[tmp.x_axis][tmp.y_axis] = '0';    //将棋盘恢复
    }
}

//反斜线
int AutoBrain::anticlinalLine()
{
    std::deque<char> pattern; //用来寻找五子棋组成的模式
    int score_1 = 0, score_2 = 0;//棋子1的分数，棋子2的分数
    int start = 10, end = 0;     // 4 - 15， 0 - 11，每条线的开始的坐标

    for(int i = 0;i < 21; ++i)
    {
        int x = start, y = end; //线上的点的坐标
        while((start + y) != 14)
        {
            //将第一个入队
            if(x == start)
            {
                pattern.push_back(chess[x][y]);
                ++x; ++y;
                continue;
            }

            //相等就入队，不相等就判断队列中的是什么模式
            if(chess[x][y] == *pattern.rbegin())
                pattern.push_back(chess[x][y]);
            else
            {
                switchPattern(score_1, score_2, x, y, pattern);
            }//if chess[x][y] == *pattern.rbegin()

            //换点
            ++x; ++y;
        }//while

        //换线
        if(start > 0)
            --start;
        else
            ++end;
        switchPattern(score_1, score_2, x, y, pattern);
        pattern.clear();
    }//for i < 21

    return score_2 - score_1;
}

//横线
int AutoBrain::horizontalLine()
{
    std::deque<char> pattern;
    int score_1 = 0, score_2 = 0;
    for(int i = 0;i < 15; ++i)//15条线
    {
        for(int j = 0;j < 15; ++j)//每条线上15个点
        {
            if(j == 0)
            {
                pattern.push_back(chess[i][j]);
                continue;
            }

            if(chess[i][j] == *pattern.rbegin())
                pattern.push_back(chess[i][j]);
            else
            {
                switchPattern(score_1, score_2, i, j, pattern);
            }//if j == *pattern.rbegin()

            //最后一个的模式
            if(j == 14)
                switchPattern(score_1, score_2, i, j, pattern);
        }//for j < 15
        pattern.clear();
    }//for i < 15

    return score_2 - score_1;
}

//斜线
int AutoBrain::slantedLine()
{
    std::deque<char> pattern;
    int score_1 = 0, score_2 = 0;
    int start = 4, end = 0;

    for(int i = 0;i < 21; ++i)
    {
        int y = start, x = end;
        while(y != end)
        {
            if(y == start)
            {
                pattern.push_back(chess[x][y]);
                ++x; --y;
                continue;
            }

            if(chess[x][y] == *pattern.rbegin())
                pattern.push_back(chess[x][y]);
            else
            {
                switchPattern(score_1, score_2, x, y, pattern);
            }//if chess[x][y] == *pattern.rbegin()

            ++x; --y;
        }//while

        if(start < 14)
            ++start;
        else
            ++end;
        switchPattern(score_1, score_2, x, y, pattern);
        pattern.clear();
    }//for i < 21

    return score_2 - score_1;
}

//竖线
int AutoBrain::verticalLine()
{
    std::deque<char> pattern;
    int score_1 = 0, score_2 = 0;
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(j == 0)
            {
                pattern.push_back(chess[j][i]);
                continue;
            }

            if(chess[j][i] == *pattern.rbegin())
                pattern.push_back(chess[j][i]);
            else
            {
                switchPattern(score_1, score_2, j, i, pattern);
            }//if j == *pattern.rbegin()

            if(j == 14)
                switchPattern(score_1, score_2, j, i, pattern);
        }//for j < 15
        pattern.clear();
    }//for i < 15
    return score_2 - score_1;
}

void  AutoBrain::switchPattern(int &score_1, int &score_2, int x, int y, std::deque<char> &pattern)
{
    if(pattern.size() == 1)
    {
        pattern.push_back(chess[x][y]);
        return; //不往下执行，直接开始下一个点
    }

    switch(*pattern.rbegin())
    {
    case '0':
        while(pattern.size() > 1)   //弹出值只剩一个
            pattern.pop_front();
        pattern.push_back(chess[x][y]);
        break;
    case '1':
        setScore(score_1, x, y, pattern);//设置分数，并且将已经计算过的分数弹出
        break;
    case '2':
        setScore(score_2, x, y, pattern);
        break;
    }
}

void AutoBrain::setScore(int &score, int x, int y, std::deque<char> &pattern)
{
    if(chess[x][y] == '1')
    {
        if(*pattern.begin() == '0')
            score += switchScore(pattern.size()-1, '1');//0x1
        else
            score += switchScore(pattern.size()-1, '2');//yx1
    }
    else if(chess[x][y] == '0')
    {
        if(*pattern.begin() == '0')
            score += switchScore(pattern.size()-1, '0');//0x0
        else
            score += switchScore(pattern.size()-1, '1');//yx0
    }//单独写成函数之后，就要判断三个值
    else
    {
        if(*pattern.begin() == '0')
            score += switchScore(pattern.size()-1, '1');//0x2
        else
            score += switchScore(pattern.size()-1, '2');//yx2
    }

    while(pattern.size() > 1)//弹出值只剩一个
        pattern.pop_front();
    pattern.push_back(chess[x][y]); //将判断的那个添加进去
}

int AutoBrain::switchScore(int num, char flag)
{
    if(num <= 1)
        return 0;

    switch (num) {
    case 2:
        switch (flag){
        case '0':
            return TWO0;
        case '1':
            return TWO1;
        case '2':
            return TWO2;
        }
    case 3:
        switch (flag){
        case '0':
            return THREE0;
        case '1':
            return THREE1;
        case '2':
            return THREE2;
        }
    case 4:
        switch (flag){
        case '0':
            return FOUR0;
        case '1':
            return FOUR1;
        case '2':
            return FOUR2;
        }
    }
}
//根据难度来进行深层次的预测运算，就是多想几步
//void AutoBrain::computeDepth()
//{

//}


#include "brain.h"

Brain::Brain() : degree(1)
{
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            chess[i][j] = '0';
        }
    }
}

Brain::~Brain()
{

}

void Brain::autoMove(int &x, int &y)
{
    chess[x][y] = '1';
    canMovePos();
    setValue();

    getBestPos(x, y);
    chess[x][y] = '2';
}

void Brain::getBestPos(int &x, int &y)
{
    posSet.sort();

    auto p_equal_num = posSet.begin();
    x = p_equal_num->x_axis;
    y = p_equal_num->y_axis;
}

void Brain::canMovePos()
{
    for(int i = 0;i < 15; ++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            if(this->chess[i][j] == '1' || this->chess[i][j] == '2')
                this->setPos(i, j);
        }
    }

    this->clearThree();
}

void Brain::setPos(int x, int y)
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

void Brain::clearThree()
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
void Brain::setValue()
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

int Brain::anticlinalLine()
{
    std::deque<char> pattern;
    int score_1 = 0, score_2 = 0;
    int start = 10, end = 0;

    for(int i = 0;i < 21; ++i)
    {
        int x = start, y = end;
        while((start + y) != 14)
        {
            if(x == start)
            {
                pattern.push_back(chess[x][y]);
                ++x; ++y;
                continue;
            }

            if(chess[x][y] == *pattern.rbegin())
                pattern.push_back(chess[x][y]);
            else
            {
                switchPattern(score_1, score_2, x, y, pattern);
            }

            ++x; ++y;
        }//while

        if(start > 0)
            --start;
        else
            ++end;
        switchPattern(score_1, score_2, x, y, pattern);
        pattern.clear();
    }//for i < 21

    return score_2 - score_1;
}

int Brain::horizontalLine()
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
int Brain::slantedLine()
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
int Brain::verticalLine()
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


void  Brain::switchPattern(int &score_1, int &score_2, int x, int y, std::deque<char> &pattern)
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

void Brain::setScore(int &score, int x, int y, std::deque<char> &pattern)
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

int Brain::switchScore(int num, char flag)
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
        break;
    case 3:
        switch (flag){
        case '0':
            return THREE0;
        case '1':
            return THREE1;
        case '2':
            return THREE2;
        }
        break;
    case 4:
        switch (flag){
        case '0':
            return FOUR0;
        case '1':
            return FOUR1;
        case '2':
            return FOUR2;
        }
        break;
    }
}

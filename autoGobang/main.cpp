#include <iostream>
#include "autobrain.h"
using namespace std;

int main()
{
    //初始化棋盘到一定的局面
    AutoBrain auCom;

    auCom.setDegree(1);

    auCom.chess[6][6] = '2';    //假定人先走
    auCom.chess[6][7] = '2';
    auCom.chess[7][7] = '1';
    auCom.chess[7][6] = '1';
//    auCom.chess[7][8] = '1';    //人走过了，开始电脑计算了

    int x = 7, y = 8;
    auCom.autoMove(x, y);
    cout << x << ":" << y << endl;

    //输出链表测试结果是否正确
    for(auto tmp : auCom.posSet)
    {
        cout << tmp.x_axis << " : " << tmp.y_axis << " := " << tmp.value << endl;
    }

    cout << "输出棋盘： \n";     //终端存在乱码
    for(int i = 0;i < 15;++i)
    {
        for(int j = 0;j < 15;++j)
        {
            cout << auCom.chess[i][j] << " ";
        }
        cout << endl;
    }

    //输出最大值的节点
//    cout << "max max ^^^^^......." << endl;
//    Pos max;
//    for(auto tmp : auCom.posSet)
//    {
//        if(tmp.value > max.value)
//            max = tmp;
//    }
//    cout << max.x_axis << ":" << max.y_axis << ":" << max.value << endl;

    return 0;
}


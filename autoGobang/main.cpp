#include <iostream>
#include "autobrain.h"
using namespace std;

int main()
{
    AutoBrain test;


//    test.setDegree('9');  //可以根据难度准确的计算出可以走的位置
//    test.setChess(0,3,'1');

      test.chess[6][5] = '1';
      test.chess[4][4] = '2';
      int x = 6,y = 6;
//    Pos step;
//    step.x_axis = 0;
//    step.y_axis = 6;
//    test.posSet.push_back(step);
//    test.setValue();
    test.autoMove(x, y);
//    test.printList();
    cout << x << ":" << y << endl;

    cout << "Hello World!" << endl;
    return 0;
}


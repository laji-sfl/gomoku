#include <QApplication>
#include "board.h"
#include "login.h"
#include <QDebug>
#include "choosepattern.h"
#include "mainwnd.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QString playNameG = QString("shilada");   //来在多个对话框之间传递
    char num = '1';       //模式

//    Board gb_board;
//    gb_board.show();

//    //登录判断对话框
//    Login *lo = new Login;
//    if(lo->exec() != QDialog::Accepted)
//        return -1;
//    playNameG = lo->playername;
//    if(!lo->flag)
//        lo->socket->close();//暂时不懂，如果close已经断开的连接会有什么影响，or nothing
//    delete lo;      //及时结束回收资源

    //选择游戏模式对话框
    ChoosePattern *cho = new ChoosePattern;
    if(cho->exec() != QDialog::Accepted)
        return -1;
    num = cho->num;
//    delete cho;   //不能释放需要让重新选择

    //游戏的主窗体
//    qDebug() << playNameG << "name";
    MainWnd wnd(num,playNameG);    /* 不理解：为什么创建一个类对象，对象构造过程中，
                     *        又创建了一个对象，但是执行的show函数，
                     *        是从QWidget继承来的，没有把子类对象传递给
                     *        父类指针，构不成多态啊。
                     * 答：创建对象时传入的this指针，和构造函数中的parent
                     *    参数，限制了他是一个控件或者一个窗口，但是内部的
                     *    实现机制还需要再一次的了解。
                     */
    wnd.show();

    return app.exec();
}

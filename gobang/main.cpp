#include <QApplication>
#include "board.h"
#include "login.h"
#include <QDebug>
#include "choosepattern.h"
#include "mainwnd.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QString playNameG;   //玩家的姓名
    char aesKey[1024] = {0};    //aes秘钥
    char playMode = '1';       //模式

    //选择游戏模式对话框
    ChoosePattern *cho = new ChoosePattern;
    if(cho->exec() != QDialog::Accepted)
        return -1;
    playMode = cho->num;
    delete cho;

    if(playMode == '2') //网络对战
    {
        //登录判断对话框
        Login *lo = new Login;
        if(lo->exec() != QDialog::Accepted) {
            delete lo;
            return -1;
        }
        playNameG = lo->playername;
        strcpy(aesKey, lo->AESkey);
        delete lo;
    }

    //游戏的主窗体
    MainWnd *wnd = new MainWnd(playMode, playNameG, aesKey);
    wnd->setAttribute(Qt::WA_DeleteOnClose, true);//窗体关闭自动释放内存
    wnd->show();

    return app.exec();
}

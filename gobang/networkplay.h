#ifndef NETWORKPLAY_H
#define NETWORKPLAY_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <string>
#include <QTcpSocket>
#include "board.h"  //父类
#include "changperinfo.h"   //修改个人信息

class NetWorkPlay : public Board
{
    Q_OBJECT
public:
    explicit NetWorkPlay(QWidget *parent = 0);

    //新增的函数
    void setMatch();
    void setTalk();
    void judgeMsg(std::string str);//根据消息的内容执行不同的动作
    void gameOver();
    void backOne(bool flag);

    //动作
    void recvMatch(std::string str);
    void recvMove(std::string str);
    void recvUndo(std::string str);
    void recvTalk(std::string str);
    void recvRival();

    //重写函数
    void setButtonLabel();
    void mouseReleaseEvent(QMouseEvent *ev);
//    void select(QString str);

public slots:
    void startTimerGame();
    void setTimeLabel();
    void dealTime0();
    void clickedPB();
    void recvChoose();

    //新增槽函数
    void clickMatch();
    void clickSend();
    void alreadyRead();
    void printErr();
    //修改个人信息
    void changInfo();

private:
    //加密编码函数
    void crypt_encode_send(QString msg, char protocolFlag);

private:
    bool        flagCon;    //是否连接，true连接
    bool        flagMat;    //是否匹配了，true匹配
    bool        flagStart;  //游戏是否开始了
    bool        flagWho;    //该谁了
    bool        flagWait;   //是否加入匹配队列
    QPushButton *changePeopleInfo;  //修改个人账号信息
    QTcpSocket  *socket;    //通信
    QPushButton *matchPlay; //开始匹配
    QPushButton *sendMsg;   //发送消息
    QLineEdit   *inputMsg;  //用来输入信息
    QTextEdit   *showMsg;   //用来显示内容
    changPerInfo *wndChangInfo; //修改个人信息的窗体

};

#endif // NETWORKPLAY_H

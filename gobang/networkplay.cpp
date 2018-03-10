#include "networkplay.h"
#include <QDebug>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QMouseEvent>

NetWorkPlay::NetWorkPlay(QWidget *parent) : Board(parent)
{
    flagCon = false;
    flagStart = false;
    flagMat = false;
    flagWait = false;
    flagWho = false;     //为ture就下棋
    setButtonLabel();
    setMatch();
    setTalk();
    this->socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(alreadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(printErr()));
    connect(socket,&QTcpSocket::disconnected,this,[=](){
        flagCon = false;
        flagMat = false;
        flagWho = false;
        flagWait = false;});
}

//tcp错误
void NetWorkPlay::printErr()
{
    showMsg->append(QString("系统：") + socket->errorString());
    socket->close();
}

//定时器时间到了
void NetWorkPlay::dealTime0()
{
    //不发给服务器了，没用
    showMsg->append(QString("系统：时间到!"));

    if(this->gr == '1') //‘1’表示点了按钮
        flagWho = false;//收到者是false，接收者是true
    else
        flagWho = true;
    gameTime = 90;  //重新计时
    setTimeg(QString("time: 1:30"));
    update();
}

void NetWorkPlay::setTimeLabel()
{
    //暂时先固定时间90秒
    int minute = 0, second = 0;
    minute = gameTime / 60;
    if(minute == 1)
        second = gameTime - 60;
    else
        second = gameTime;

    //发射信号
    if(gameTime == 0)
        emit timeEqual0();

    --gameTime;
    QString str = QString("time:%1:%2").arg(minute).arg(second);

    //根据对手选择要设置的定时器
    if(flagWho)
    {
        setTimeg(str);
    }
    else
        setTimer(str);
}

//一系列的动作
void NetWorkPlay::recvMatch(std::string str)
{
    if(str[1] == '0')
    {
        if(str[2] == '1')
        {
            //服务器将他加入了匹配队列
            flagWait = true;
            flagMat = false;
            showMsg->append(QString("系统：正在匹配。"));
        }
        else
        {
            //匹配失败
            flagMat = false;
            showMsg->append(QString("系统：匹配失败！"));
        }
    }
    else if(str[1] == '1')
    {
        //匹配成功
        flagMat = true;
        flagWait = false;
        showMsg->append(QString("系统：成功匹配。"));
        std::string heNa(str.begin() + 2,str.end());
        oneGame.heName = QString(heNa.c_str());

        QString he = QString("昵称：%1").arg(oneGame.heName);
        QString my = QString("昵称：%1").arg(oneGame.myName);
    }
}
void NetWorkPlay::recvMove(std::string str)
{
    if(!flagStart){
    //判断是否是开始信号
        if(str[1] == '-'){
            Board::startTimerGame();
            this->gr = '2';     //接收者是2
            flagWho = false;    //接收者后下棋
            flagStart = true;   //开始了
            showMsg->append(QString("系统：游戏已开始"));
            this->setNameg(QString("name:") + oneGame.myName);
            this->setNamer(QString("name:") + oneGame.heName);//写在这里是因为真的不知道什么原因，写在收到匹配函数中的时候没有效果
            return;
        }
    }

    if(str[1] == '-')
        return;

    //根据谁先点的开始谁的子的颜色
    char myGR;
    if(this->gr == '1')
        myGR = '2';
    else if(this->gr == '2')
        myGR = '1';

    //提取位置坐标，并写入oneGame,重绘屏幕
    QString xPos,yPos;
    xPos.append(QChar(str[1]));
    xPos.append(QChar(str[2]));
    yPos.append(QChar(str[3]));
    yPos.append(QChar(str[4]));

    oneGame.stonePos[xPos.toInt()][yPos.toInt()] = myGR;

    if(judgeWin())  //判断输赢
        gameOver();

    step *newSt = new step; //加入步骤链表
    newSt->gr = myGR;
    newSt->x = xPos.toInt();
    newSt->y = yPos.toInt();
    oneGame.stepList->append(newSt);

    gameTime = 90;  //重新计时
    setTimer(QString("time: 1:30"));
    update();

    flagWho = true;     //接收到对面的之后就可以继续下棋的
}
void NetWorkPlay::recvUndo(std::string str)
{
    // 1表示同意悔棋，0表示请求悔棋
    if(str[1] == '1')
    {
        backTwo(true);  //玩家下
    }
    else if(str[1] == '0')
    {
        //弹出对话框，让选择
        auto ret = QMessageBox::warning(this,"悔棋", "是否同意悔棋？",QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
        if(ret == QMessageBox::Yes)
        {
            QByteArray bufNo;
            bufNo.append(QString("51"));
            socket->write(bufNo);

            auto tmp = oneGame.stepList->at(oneGame.stepList->length() - 1);
            auto tmp1 = oneGame.stepList->at(oneGame.stepList->length() - 2);

            if(tmp->gr == this->gr)
            {
                //弹出我的和他的，让他走
                oneGame.stonePos[tmp->x][tmp->y] = '0';
                oneGame.stonePos[tmp1->x][tmp1->y] = '0';
                oneGame.stepList->removeLast();
                oneGame.stepList->removeLast();
                delete tmp;
                delete tmp1;

                flagWho = false;
                gameTime = 90;  //重新计时
                setTimer(QString("time: 1:30"));
                update();
            }
            else
            {
                oneGame.stonePos[tmp->x][tmp->y] = '0';
                oneGame.stepList->removeLast();
                delete tmp;

                flagWho = false;
                gameTime = 90;
                setTimer(QString("time: 1:30"));
                update();
            } //对手下
        }
        else    //不同意
        {
            QByteArray bufNo;
            bufNo.append(QString("52"));    // 2表示拒绝
            socket->write(bufNo);
        }
    }
    else
        showMsg->append(QString("系统：对方拒绝了～"));
}
void NetWorkPlay::recvTalk(std::string str)
{
    QString ss = QString(str.c_str());
    ss.remove(0,1);
    showMsg->append(oneGame.heName + ":" + ss);
}

//回滚两步
void NetWorkPlay::backTwo(bool flag)
{
    //true表示玩家继续下棋，false表示对手继续

    //true表示谁按得按钮

    //直接根据步骤链表修改内存棋盘，然后重绘
    auto tmp = oneGame.stepList->at(oneGame.stepList->length() - 1);
    auto tmp1 = oneGame.stepList->at(oneGame.stepList->length() - 2);

    if(tmp->gr != this->gr)
    {
        //弹出我的和他的，让他走
        oneGame.stonePos[tmp->x][tmp->y] = '0';
        oneGame.stonePos[tmp1->x][tmp1->y] = '0';
        oneGame.stepList->removeLast();
        oneGame.stepList->removeLast();
        delete tmp;
        delete tmp1;

        flagWho = flag;
        gameTime = 90;  //重新计时
        setTimer(QString("time: 1:30"));
        update();
    }
    else
    {
        oneGame.stonePos[tmp->x][tmp->y] = '0';
        oneGame.stepList->removeLast();
        delete tmp;

        flagWho = flag;
        gameTime = 90;
        setTimer(QString("time: 1:30"));
        update();
    }
}

//悔棋按钮,槽函数
void NetWorkPlay::clickedPB()
{
    if(oneGame.stepList->length() < 2) //没有棋子和只有一个子的时候不允许悔棋
        return;

    //发送悔棋请求
    QByteArray buf;
    buf.append(QString("50"));
    socket->write(buf);
    showMsg->append(QString("系统：请求已发送。"));
}

//判断消息
void NetWorkPlay::judgeMsg(std::string str)
{
    switch(str[0])
    {
    case '2':
        recvMatch(str);
        break;
    case '4':
        recvMove(str);
        break;
    case '5':
        recvUndo(str);
        break;
    case '6':
        recvTalk(str);
        break;
    default:
        qDebug() << "检查一下服务器代码";
        break;
    }
}

//鼠标事件
void NetWorkPlay::mouseReleaseEvent(QMouseEvent *ev)
{
    //不该自己下就不能点
    if(!flagWho)
        return;
    //不是左键点击不予理会
    if(ev->button() != Qt::LeftButton)
        return;
    //没开始游戏不能点棋盘
    if(!flagStart)
        return;

    int x = -1, y = -1;

    //计算出点击的坐标
    computePos(ev->pos(), x, y);

    if(oneGame.stonePos[x][y] != '0')   //点击无效
        return;

    //变化棋盘
    setTimeg(QString("time:1:30"));
    this->gameTime = 90;
    setGameDataPosStep(x, y, gr);

    //发送给对面
    QByteArray buf;
    buf.append('4');
    if(x < 10)
        buf.append(QString("0%1").arg(x));
    else
        buf.append(QString("%1").arg(x));
    if(y < 10)
        buf.append(QString("0%1").arg(y));
    else
        buf.append(QString("%1").arg(y));

    socket->write(buf);
    flagWho = false;

    update();   //重新绘制
    if(judgeWin())
        gameOver();
}

//开始按钮
void NetWorkPlay::startTimerGame()
{
    if(!flagMat)
        return;
    if(flagStart)
        return;
    if(!flagCon)
    {
        showMsg->append(QString("系统：请先匹配"));
        return;
    }

    flagStart = true;
    flagWho = true;
    QByteArray buf;
    buf.append('4');
    buf.append('-');
    socket->write(buf);
    Board::startTimerGame();    //启动定时器开始游戏
    showMsg->append(QString("系统：游戏已开始"));
    this->gr = '1'; /*谁先点击的开始按钮谁是1，另一个就是2*/
    this->setNamer(QString("name:") + oneGame.heName);
    this->setNameg(QString("name:") + oneGame.myName);
}


//读取消息
void NetWorkPlay::alreadyRead()
{
    QByteArray buf = socket->readAll();

    //根据消息进行判断
    judgeMsg(buf.toStdString());
}

//设置匹配按钮
void NetWorkPlay::setMatch()
{
    this->matchPlay = new QPushButton(QString("匹配"), this);
    matchPlay->move(100, 480);
    matchPlay->setMaximumSize(80, 40);
    matchPlay->setMinimumSize(80, 40);
    connect(matchPlay, SIGNAL(clicked(bool)),this,SLOT(clickMatch()));
}

//点击开始匹配
void NetWorkPlay::clickMatch()
{
    if(flagMat)
        return;
    if(flagWait)
        return;
    if(flagStart)
        return;

    //建立网络连接
    if(!flagCon)
        socket->connectToHost(QHostAddress("127.0.0.1"), 9996);
    flagCon = true;

    //发送开始匹配的信息
    QByteArray msg;
    msg.append('2');
    msg.append(oneGame.myName);
    socket->write(msg);

    //发送系统通知
    showMsg->append(QString("系统：正在匹配对手"));
}

//发送
void NetWorkPlay::clickSend()
{
    //读取消息并发送,同时显示在中间
    QString msg = inputMsg->text();
    if(msg.isEmpty())
        return;
    QByteArray buf;
    buf.append('6');
    buf.append(msg);
    if(flagCon)     //连接了才发送
        socket->write(buf);
    showMsg->append(oneGame.myName + ": " + msg);
    inputMsg->clear();
}

//聊天
void NetWorkPlay::setTalk()
{
    this->sendMsg = new QPushButton(QString("发送"),this);
    sendMsg->move(580, 485);
    sendMsg->setMaximumSize(60,30);
    sendMsg->setMinimumSize(60,30);
    connect(sendMsg, SIGNAL(clicked(bool)),this,SLOT(clickSend()));

    this->inputMsg = new QLineEdit(this);
    inputMsg->move(400, 485);
    inputMsg->setMaximumSize(170,30);
    inputMsg->setMinimumSize(170,30);

    this->showMsg = new QTextEdit(this);
    showMsg->move(490,205);
    showMsg->setMaximumSize(150,110);
    showMsg->setMinimumSize(150,110);
    //设置只读
    showMsg->setReadOnly(true);
}

//设置悔棋和label
void NetWorkPlay::setButtonLabel()
{
    //显示悔棋按钮
    this->pullBack = new QPushButton(this);
    this->pullBack->setText(QString("悔棋"));
    this->pullBack->setMaximumSize(80,40);
    this->pullBack->setMinimumSize(80,40);
    this->pullBack->move(200,480);
    connect(pullBack,&QPushButton::clicked,this,&Board::clickedPB);

    //显示自己的昵称头像和对方的昵称头像
    gname = new QLabel(this);
    gpicture = new QLabel(this);
    gtime = new QLabel(this);
    rtime = new QLabel(this);
    rpicture = new QLabel(this);
    rname = new QLabel(this);
    this->setNameg(QString("昵称：") + oneGame.myName);
    this->setPictureg(QString("kk"));
    this->setTimeg(QString("time: 0:00"));
    this->setNamer(QString("昵称：") + oneGame.heName);
    this->setPicturer(QString("asd"));
    this->setTimer(QString("time: 0:00"));
    update();//重新绘制
}

//游戏结束之后
void NetWorkPlay::gameOver()
{
    //停止定时器,点击开始按钮会再一次创建
    delete this->timer;

    //将开始状态复位
    flagStart = false;

    //清空链表，棋盘
    for(auto tmp = oneGame.stepList->begin();oneGame.stepList->end() != tmp; ++tmp)
    {
        delete (*tmp);
    }
    oneGame.stepList->clear();
    for(int i = 0;i < 15;++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            oneGame.stonePos[i][j] = '0';
        }
    }

    //提示是否重新匹配
    int ret = QMessageBox::warning(this,"GameOver","是否更换对手？",QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
    if(QMessageBox::Yes == ret)
    {
        /*正常来讲，如果对方先于自己点了是，那么服务器会收到那个人的数据，
         * 然后将两个人的tcp都关闭，那么自己就发不出去消息了，没有影响
         * 就算发出去了，服务器已经将资源释放了（服务器有锁，总会有一个先来后到）
         * 没有影响。
         */

        QByteArray buf;
        buf.append('7');
        buf.append(oneGame.myName);

        flagMat = false;
        flagWho = false;
        flagWait = false;

        if(flagCon) //连接着服务器才能发送
            socket->write(buf);

        showMsg->append(QString("系统：请点击按钮开始匹配"));
    }
    update();
}

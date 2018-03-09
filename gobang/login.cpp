#include "login.h"
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QHostAddress>

Login::Login(QWidget *parent) : QDialog(parent)
{
    //将控件初始化
    this->setWidget();
    flag2 = false;  // true 表示注册成功了

    //信号与槽
    connect(this->sign_in, SIGNAL(clicked(bool)), this, SLOT(inClicked()));
    connect(this->sign_on, SIGNAL(clicked(bool)), this, SLOT(onClicked()));

    //初始化socket
    this->socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress("127.0.0.1"), 9996);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readSock()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(printErr()));
//    connect(socket, SIGNAL(disconnected()),this,SLOT(setCloseBool()));//服务器会主动断开连接，当返回11之后。
}

//void Login::setCloseBool()
//{
//    qDebug() << "断开连接诶";
//}

/*
 * 结束程序，需要了解，执行close的原理是否会释放内存，是否结束程序，是否向下执行
 *   这里不懂原理，需不需要释放这个对话框的资源，还是会自动释放。
 */
//void Login::closeEvent(QCloseEvent *q)
//{
//    QDialog::closeEvent(q);
//    exit(200);
//}

void Login::printErr()
{
//    if(flag)   //断开连接之后就不接受错误了
//    {
//        qDebug() << "error:" << flag;
//        return;
//    }

//    qDebug() << "error:" << flag;

    QMessageBox::warning(this, "waring", socket->errorString(), QMessageBox::Yes, QMessageBox::Yes);
}

//读取服务器的数据
void Login::readSock()
{
    QByteArray str = socket->readAll();
//    qDebug() << "jie shou:" << str;
    if(str.at(0) == '1')
    {
        if(str.at(1) == '1')
        {
            accept();
        }
        else
        {
            QMessageBox::warning(this, "waring", "please first register!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    else if(str.at(0) == '3')
    {
        if(str.at(1) == '1')
        {
            QMessageBox::warning(this, "tip", "register victory!", QMessageBox::Yes, QMessageBox::Yes);
            flag2 = true;
        }
        else
        {
            QMessageBox::warning(this, "waring", "name all exist!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    else
    {
        QMessageBox::warning(this, "waring", "can not login!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
}

//槽函数登录
void Login::inClicked()
{
    //获取控件的值,并判断合法性
    QString strName = this->name->text();
    QString strPwd = this->pwd->text();

    //服务器的要求
    if(strName.length() > 16 || strPwd.length() > 16)
    {
        QMessageBox::warning(this, "waring", "can not be empty!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    playername = strName; //记录玩家名字

    if(strName.isEmpty() || strPwd.isEmpty())
    {
        QMessageBox::warning(this, "waring", "can not be empty!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //向服务器发送验证数据
    QByteArray str;
    str.append('1');
    str.append(strName);
    str.append(':');
    str.append(strPwd);
    str.append('&');
//    qDebug() << "send:" << str;
    socket->write(str);
}

//槽函数注册
void Login::onClicked()
{
    if(flag2)   //已经注册了
        return;

    //获取控件的值,并判断合法性
    QString strName = this->name->text();
    QString strPwd = this->pwd->text();

    if(strName.isEmpty() || strPwd.isEmpty())
    {
        QMessageBox::warning(this, "warning", "direct input name and password click button!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //向服务器发送注册数据
    QByteArray str;
    str.append('3');
    str.append(strName);
    str.append(':');
    str.append(strPwd);
    str.append('&');
    qDebug() << "send:" << str;
    socket->write(str);
}

//设置控件
void Login::setWidget()
{
    this->vBoxLayout = new QVBoxLayout(this);
    this->formLayout = new QFormLayout();
    this->hBoxLayout = new QHBoxLayout();
    this->nameLabel = new QLabel(QString("name:"));
    this->pwdLabel = new QLabel(QString("password:"));
    this->name = new QLineEdit();
    this->pwd = new QLineEdit();
    pwd->setEchoMode(QLineEdit::Password);
    this->sign_in = new QPushButton(QString("登录"));
    this->sign_on = new QPushButton(QString("注册"));

    formLayout->addWidget(nameLabel);
    formLayout->addWidget(name);
    formLayout->addWidget(pwdLabel);
    formLayout->addWidget(pwd);

    hBoxLayout->addWidget(sign_in);
    hBoxLayout->addWidget(sign_on);

    vBoxLayout->addLayout(formLayout);
    vBoxLayout->addLayout(hBoxLayout);
}

Login::~Login()
{
    /*
     *  布局控件在添加控件的时候会将加入的控件变为内部的子控件，
     *  这样在最后结束的时候会将控件自动的回收，而不需要我们做什么。
     */
//    delete formLayout;
//    delete pwd;
//    delete pwdLabel;
//    delete hBoxLayout;
//    delete sign_in;
//    delete sign_on;

    //断开连接
//    socket->close();
}


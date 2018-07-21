#include "login.h"
#include <QMessageBox>
#include <QDebug>
#include "encryption.h"
#include <QByteArray>
#include <QHostAddress>

Login::Login(QWidget *parent) : QDialog(parent)
{
    //将控件初始化
    this->setWidget();
    flag2 = false;  // true 表示注册成功了
    tcpError = false;   //连接没有错误
    recvAESkey = false; //是否收到了AES秘钥
    memset(AESkey, 0, 1024);

    //创建公钥
    createRSAkey();

    //信号与槽
    connect(this->sign_in, SIGNAL(clicked(bool)), this, SLOT(inClicked()));
    connect(this->sign_on, SIGNAL(clicked(bool)), this, SLOT(onClicked()));

    //初始化socket
    this->socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress("127.0.0.1"), 9996);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readSock()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(printErr()));
}

/*
 * 结束程序，需要了解，执行close的原理是否会释放内存，是否结束程序，是否向下执行
 *   这里不懂原理，需不需要释放这个对话框的资源，还是会自动释放。
 */
//tcp连接出现的问题他都会弹出
void Login::printErr()
{
    QMessageBox::warning(this, "waring", socket->errorString(), QMessageBox::Yes, QMessageBox::Yes);
    tcpError = true;
}

//读取服务器的数据
void Login::readSock()
{
    QByteArray str = socket->readAll();
    int strlens = str.length();
    qDebug() << str << strlens;

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
    else if(str.at(0) == 'C')
    {
        //接收服务器公钥,将客户端公钥发送过去
        char *key;
        char file[20] = "./pub_str_key";
        readRSAKey(file, &key);

        QByteArray msg;
        msg.append('C');
        msg.append(key);
        socket->write(msg);
        free(key);
    }
    else if(str.at(0) == 'D')
    {
        //接收aes秘钥并保存起来
        char *key;  //RSA秘钥
        char beDecode[2048] = {0};//解码后
        char cipherText[2048] = {0};//接收到的密文
        char *p = cipherText;       //操作密文的指针
        char file[20] = "./pri_str_key";

        memcpy(cipherText, str.toStdString().c_str(), strlens);//密文
        base64_decode(p+1, strlens-1, beDecode);//密文解码
        readRSAKey(file, &key);//读取私钥
        pricrypt(key, beDecode, AESkey);//解密,加1是为了取出前面的D
        qDebug() << "AES KEY:" << AESkey;
        free(key);
        recvAESkey = true;
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
    if(tcpError)
        exit(0);
    if(!recvAESkey)
        return;

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
    str.append(strName);
    //name和pwd必须保证都是16字节
    for (int i = 0;i < 16 - strName.length(); ++i)
        str.append('0');
    str.append(strPwd);
    for (int i = 0;i < 16 - strName.length(); ++i)
        str.append('0');

    //加密
    char cipher[1024] = {0};
    char plain[1024] = {0};
    memcpy(plain, str.toStdString().c_str(), str.length());

    int ret = aesCrypt(plain, cipher, AESkey);
    memset(plain, 0, 1024);
    base64_encode(cipher, ret, plain + 1);

    plain[0] = '1';
    qDebug() << "plain:" << plain;
    socket->write(plain);
}

//槽函数注册
void Login::onClicked()
{
    if(tcpError)
        exit(0);
    if(!recvAESkey)
        return;
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
    str.append(strName);
    for (int i = 0;i < 16 - strName.length(); ++i)
        str.append('0');
    str.append(strPwd);
    for (int i = 0;i < 16 - strName.length(); ++i)
        str.append('0');

    //加密
    char cipher[1024] = {0};
    char plain[1024] = {0};
    memcpy(plain, str.toStdString().c_str(), str.length());

    int ret = aesCrypt(plain, cipher, AESkey);
    memset(plain, 0, 1024);
    base64_encode(cipher, ret, plain + 1);

    plain[0] = '3';
    qDebug() << "plain:" << plain;
    socket->write(plain);
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
}


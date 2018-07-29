#include "changperinfo.h"
#include <QMessageBox>
#include <QFile>
#include <QHostAddress>

changPerInfo::changPerInfo(QString name, char *aesKey, QWidget *parent) : QWidget(parent)
{
    this->myname = name;
    this->myaes = aesKey;

    this->socketChange = new QTcpSocket(this);
    socketChange->connectToHost(QHostAddress("127.0.0.1"), 9996);
    connect(socketChange, SIGNAL(readyRead()), this, SLOT(readSock()));
    connect(socketChange, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(printErr()));

    this->setWindowTitle("更新个人信息");
    this->setMinimumSize(280, 260);

    this->changPwd = new QPushButton("更改密码", this);
    this->changPwd->move(60, 115);

    this->changImg = new QPushButton("更改头像", this);
    this->changImg->move(70, 200);

    this->oldPwd = new QLineEdit(this);
    this->oldPwd->move(100, 20);
    this->newPwd = new QLineEdit(this);
    this->newPwd->setEchoMode(QLineEdit::Password);
    this->newPwd->move(100, 50);
    this->newPwd2 = new QLineEdit(this);
    this->newPwd2->setEchoMode(QLineEdit::Password);
    this->newPwd2->move(100, 80);

    this->oldPwdLable = new QLabel("旧密码:", this);
    this->oldPwdLable->move(30, 20);
    this->newPwdLable = new QLabel("新密码:", this);
    this->newPwdLable->move(30, 50);
    this->newPwd2Lable = new QLabel("确认密码:", this);
    this->newPwd2Lable->move(30, 80);

    this->imgPath = new QLineEdit(this);
    this->imgPath->move(40, 170);
    this->imgPathLable = new QLabel("头像图片的路径:", this);
    this->imgPathLable->move(40, 150);

    connect(this->changPwd, SIGNAL(clicked(bool)), this, SLOT(changePwd()));
    connect(this->changImg, SIGNAL(clicked(bool)), this, SLOT(changeImg()));
}

void changPerInfo::changePwd()
{
    QString strOldPwd = oldPwd->text();
    this->strNewPwd = newPwd->text();
    QString strNewPwd2 = newPwd2->text();

    //不能为空
    if (strOldPwd.isEmpty() || strNewPwd.isEmpty() || strNewPwd2.isEmpty())
    {
        QMessageBox::warning(this, "waring", "no empty!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //新密码两个要相同
    if (strNewPwd != strNewPwd2)
    {
        QMessageBox::warning(this, "waring", "new password different!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //旧密码和新密码相同
    if (strNewPwd == strOldPwd)
    {
        QMessageBox::warning(this, "waring", "new password same as old!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //长度
    if (strNewPwd.length() > 16)
    {
        QMessageBox::warning(this, "waring", "password too length!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //test:
    myname = QString("laji");

    //验证输入的旧密码是否正确，重新登录，在收到服务器的回应之后在更新密码
    this->sendMsgToServ(strOldPwd, '1');
}

void changPerInfo::changeImg()
{
    /*
     *  文件传输需要重新连接到新的文件服务器上，暂时不予实现
     */

    QString strimg = imgPath->text();

    if (strimg.isEmpty())
    {
        QMessageBox::warning(this, "waring", "no empty!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    qDebug() << "wait file server be created";
    QMessageBox::warning(this, "waring", "wait file server be created", QMessageBox::Yes, QMessageBox::Yes);

    //打开文件
//    QFile file(strimg);
//    if(!file.open(QIODevice::ReadOnly))
//    {
//        QMessageBox.warning(this, "waring", "can't open file!", QMessageBox::Yes, QMessageBox::Yes);
//        return;
//    }
//    if(file.size() > 2048)
//    {
//        QMessageBox.warning(this, "waring", "can't open file!", QMessageBox::Yes, QMessageBox::Yes);
//        return;
//    }

    //发送给服务器
//    QByteArray str;
//    str.append(myname);
//  //name和pwd必须保证都是16字节
//    for (int i = 0;i < 16 - myname.length(); ++i)
//        str.append('0');
//    str.append(file.readAll());

//    //加密
//    char cipher[] = {0};
//    char plain[1024] = {0};
//    memcpy(plain, str.toStdString().c_str(), str.length());

//    int ret = aesCrypt(plain, cipher, myaes);
//    memset(plain, 0, 1024);
//    base64_encode(cipher, ret, plain + 1);

//    plain[0] = 'A';
//    socketChange->write(plain);
}

void changPerInfo::readSock()
{
    //读取服务器的返回值
    QByteArray str = socketChange->readAll();
    qDebug() << "服务器返回：" << str;

    if (str.at(0) == 'A')
    {
        if(str.at(1) == '1' && loginFlag)//旧密码正确服务器也成功
        {
            QMessageBox::warning(this, "message", "change password victory!", QMessageBox::Yes, QMessageBox::Yes);
        }
        else
        {
            QMessageBox::warning(this, "waring", "change password defeat!", QMessageBox::Yes, QMessageBox::Yes);
        }
    }
    else if (str.at(0) == '1')
    {
        if(str.at(1) == '1')//登录成功，可以更改密码
        {
            loginFlag = true;
            sendMsgToServ(this->strNewPwd, 'A');
            qDebug() << "upadte pwd";
        }
        else
        {
            QMessageBox::warning(this, "waring", "old password error!", QMessageBox::Yes, QMessageBox::Yes);
        }
    }
    else
    {
        qDebug() << "error in recv socket change password";
    }
}

void changPerInfo::printErr()
{
    //打印错误信息
    QMessageBox::warning(this, "waring", socketChange->errorString(), QMessageBox::Yes, QMessageBox::Yes);
}

void changPerInfo::closeEvent(QCloseEvent *)
{
    //断开连接
    this->socketChange->close();

    delete this;    //释放new出来的窗体，会自动调用析构函数
    //～changePerInfo();主动调用析构函数，这两种方法有什么区别
}

void changPerInfo::sendMsgToServ(QString pwd, char flag)
{
    //发送给服务器
    QByteArray str;
    str.append(myname);
    //name和pwd必须保证都是16字节
    for (int i = 0;i < 16 - myname.length(); ++i)
        str.append('0');
    str.append(pwd);
    for (int i = 0;i < 16 - pwd.length(); ++i)
        str.append('0');

    //加密
    char cipher[1024] = {0};
    char plain[1024] = {0};
    memcpy(plain, str.toStdString().c_str(), str.length());

    int ret = aesCrypt(plain, cipher, myaes);
    memset(plain, 0, 1024);
    base64_encode(cipher, ret, plain + 1);

    plain[0] = flag;
    socketChange->write(plain);
}

changPerInfo::~changPerInfo()
{
}


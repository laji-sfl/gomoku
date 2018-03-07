#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
//#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTcpSocket>

class Login : public QDialog
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    void setWidget();
    QTcpSocket *socket;     //客户端socket
    QString playername;
    bool flag,flag2;

//    void closeEvent(QCloseEvent *);

public slots:
    void inClicked();   //注册的槽函数
    void onClicked();   //登录的槽函数
    void readSock();    //读取服务器发来的消息
    void setCloseBool();
    void printErr();
//    void printErr1();
//    void exitEXE();     //退出程序

private:
    QFormLayout *formLayout;//便签输入布局
    QHBoxLayout *hBoxLayout;//水平布局
    QVBoxLayout *vBoxLayout;//垂直布局
    QPushButton *sign_in;   //注册
    QPushButton *sign_on;   //登录
    QLineEdit *name;        //名字
    QLineEdit *pwd;         //密码    /**暂时不加密**/
    QLabel *nameLabel;      //名字和密码标签
    QLabel *pwdLabel;

};

#endif // LOGIN_H

#ifndef CHANGPERINFO_H
#define CHANGPERINFO_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QTcpSocket>
#include "encryption.h"
#include <QLineEdit>

class changPerInfo : public QWidget
{
    Q_OBJECT
public:
    explicit changPerInfo(QString name, char *aesKey, QWidget *parent = 0);
    ~changPerInfo();

public slots:
    void changePwd();   //更改密码
    void changeImg();   //更换头像
    void readSock();
    void printErr();
    void closeEvent(QCloseEvent *);

private:
    //辅助函数，加密编码发送给服务器
    void sendMsgToServ(QString pwd, char flag);

private:
    QPushButton *changPwd;
    QPushButton *changImg;
    QLineEdit *newPwd;
    QLineEdit *oldPwd;
    QLineEdit *newPwd2;
    QLineEdit *imgPath;
    QLabel *newPwdLable;
    QLabel *oldPwdLable;
    QLabel *newPwd2Lable;
    QLabel *imgPathLable;
    QString myname;
    QString strNewPwd;
    char *myaes;
    QTcpSocket *socketChange;
    bool loginFlag;
};

#endif // CHANGPERINFO_H

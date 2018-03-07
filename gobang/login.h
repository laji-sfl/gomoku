#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QWidget>

class Login : public QDialog
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = 0);
};

#endif // LOGIN_H

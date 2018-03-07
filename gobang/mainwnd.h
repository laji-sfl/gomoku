#ifndef MAINWND_H
#define MAINWND_H

#include <QWidget>

class MainWnd : public QWidget
{
    Q_OBJECT
public:
    explicit MainWnd(char num = '1', QString name = "", QWidget *parent = 0);
};

#endif // MAINWND_H

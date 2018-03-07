#ifndef NETWORKPLAY_H
#define NETWORKPLAY_H

#include <QWidget>
#include "board.h"  //父类

class NetWorkPlay : public Board
{
    Q_OBJECT
public:
    explicit NetWorkPlay(QWidget *parent = 0);
};

#endif // NETWORKPLAY_H

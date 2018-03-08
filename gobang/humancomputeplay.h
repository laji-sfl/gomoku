#ifndef HUMANCOMPUTEPLAY_H
#define HUMANCOMPUTEPLAY_H

#include "board.h"

class humanComputePlay : public Board
{
    Q_OBJECT
public:
    explicit humanComputePlay(QWidget *parent = 0);

    void setButtonLabel();
};

#endif // HUMANCOMPUTEPLAY_H

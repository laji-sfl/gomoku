#ifndef HUMANCOMPUTEPLAY_H
#define HUMANCOMPUTEPLAY_H

#include "board.h"

class humanComputePlay : public Board
{
    Q_OBJECT
public:
    explicit humanComputePlay(QWidget *parent = 0);

    void setButtonLabel();
    //机器走子
    void computeMove();
    void mouseReleaseEvent(QMouseEvent *ev);
    void gameOver();

signals:
    void sendCompute();

public slots:
    void clickedPB();
    void startTimerGame();
    void dealTime0();
    void recvCompute();

private:
    bool flagWho;
    bool flagStart;
};

#endif // HUMANCOMPUTEPLAY_H

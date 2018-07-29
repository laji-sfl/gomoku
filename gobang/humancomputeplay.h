#ifndef HUMANCOMPUTEPLAY_H
#define HUMANCOMPUTEPLAY_H

#include "board.h"
#include "brain.h"
#include <QPushButton>
#include <QLineEdit>

class humanComputePlay : public Board
{
    Q_OBJECT
public:
    explicit humanComputePlay(QWidget *parent = 0);
    ~humanComputePlay();

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
    void changDegree();

private:
    Brain *autoBrain;
    bool flagWho;
    bool flagStart;
    QPushButton *changeDegree;
    QLineEdit *degreeNum;
};

#endif // HUMANCOMPUTEPLAY_H

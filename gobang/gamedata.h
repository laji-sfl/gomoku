#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include <QList>

/*
 *  用来记录游戏双方之间的数据。
 *  内存棋布，15*15的二维数组来记录棋子的坐标和敌我
 *  记录游戏双方玩家的昵称
 *  记录游戏双方下棋的步骤，用于悔棋
 */

//步骤结构体
struct step
{
    int x;      //横坐标
    int y;      //纵坐标
    char gr;   //敌我双方，green，red
};

class GameData
{
public:
    GameData();

    char stonePos[15][15];
    QString myName, heName;
    QList<step> stepList;
};

#endif // GAMEDATA_H

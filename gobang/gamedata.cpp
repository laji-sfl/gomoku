#include "gamedata.h"

GameData::GameData()
{
    //异常退出的原因，就是因为内存大小写错了
    memset(this->stonePos, 0, 1 * 15 * 15);
}



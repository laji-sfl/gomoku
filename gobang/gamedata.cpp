#include "gamedata.h"

GameData::GameData()
{
    //异常退出的原因，就是因为内存大小写错了
    for(int i = 0;i < 15;++i)
    {
        for(int j = 0;j < 15; ++j)
        {
            this->stonePos[i][j] = '0';
        }
    }
    aes_key = NULL;
}



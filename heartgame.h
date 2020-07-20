#ifndef HEARTGAME_H
#define HEARTGAME_H


#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <Winsock2.h> //Socket的函数调用　
#include <windows.h>
using namespace std;

#pragma comment (lib, "ws2_32")
//玩家数：4 最大牌数: 13 因为需要在头部填充标识符使用15作为maxsize
#define MaxPlayer 4
#define MAX_CARD 15

class player {
public:
    SOCKET client;
};

class HeartGame
{
public:
    //初始化游戏
    HeartGame();
    //洗牌
    int GetCards();
    //发牌,同时得到第一个出牌的玩家
    int SendCard(int i);
    //    vector<int> GetCardValue(Player p);
    //    Player GetFirstPlayer();
        //获得牌值
    vector<int> GetCardValue(int p);
    //获得第一个出牌玩家
    int GetFirstPlayer();
    int GetMaxPlayer(int card[]);
    //    int GetMaxPlayer(CardWidget** card, int first);
    //    void PointCount(CardWidget** card, int first);
    int* GetPlayerPoint();
    int* GetTotalPoint();
    void InitGamePoint();
    //    QString GetWinner();
    int* PointCount();
    int GetWinner();

private:
    char P1CardValue[MAX_CARD];                //玩家1的手牌值
    char P2CardValue[MAX_CARD];                //玩家2的手牌值
    char P3CardValue[MAX_CARD];                //玩家3的手牌值
    char P4CardValue[MAX_CARD];                //玩家4的手牌值
    vector<int> Cards;
    int GamePoint[MaxPlayer];               //玩家的当前得分
    int TotalPoint[MaxPlayer];              //玩家的累计得分
    int next_player;                        //下一轮出牌的玩家 
};

#endif // HEARTGAME_H

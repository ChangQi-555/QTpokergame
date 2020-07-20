#ifndef HEARTGAME_H
#define HEARTGAME_H


#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <Winsock2.h> //Socket�ĺ������á�
#include <windows.h>
using namespace std;

#pragma comment (lib, "ws2_32")
//�������4 �������: 13 ��Ϊ��Ҫ��ͷ������ʶ��ʹ��15��Ϊmaxsize
#define MaxPlayer 4
#define MAX_CARD 15

class player {
public:
    SOCKET client;
};

class HeartGame
{
public:
    //��ʼ����Ϸ
    HeartGame();
    //ϴ��
    int GetCards();
    //����,ͬʱ�õ���һ�����Ƶ����
    int SendCard(int i);
    //    vector<int> GetCardValue(Player p);
    //    Player GetFirstPlayer();
        //�����ֵ
    vector<int> GetCardValue(int p);
    //��õ�һ���������
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
    char P1CardValue[MAX_CARD];                //���1������ֵ
    char P2CardValue[MAX_CARD];                //���2������ֵ
    char P3CardValue[MAX_CARD];                //���3������ֵ
    char P4CardValue[MAX_CARD];                //���4������ֵ
    vector<int> Cards;
    int GamePoint[MaxPlayer];               //��ҵĵ�ǰ�÷�
    int TotalPoint[MaxPlayer];              //��ҵ��ۼƵ÷�
    int next_player;                        //��һ�ֳ��Ƶ���� 
};

#endif // HEARTGAME_H

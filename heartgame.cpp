
#include "heartgame.h"


using namespace std;

#pragma comment (lib, "ws2_32")

player Player[4];
int first_player = 0;
HeartGame::HeartGame()
{
    for (int i = 0; i < MaxPlayer; i++) {
        GamePoint[i] = 0;
        TotalPoint[i] = 0;
    }
    next_player = 0;
};
int HeartGame::GetCards()
{
    vector<int> cards;
    for (int i = 1; i <= 52; i++) {
        cards.push_back(i);
    }

    //设置随机数
    srand(unsigned(time(0)));
    //洗三次
    for (int j = 0; j < 3; j++) {
        //随机交换，实现洗牌
        for (int i = 0; i < 52; i++) {
            swap(cards[i], cards[rand() % 52]);
        }

    }
    Cards = cards;
    P1CardValue[0] = P2CardValue[0] = P3CardValue[0] = P4CardValue[0] = 'C';


    for (int i = 0; i < 13; i++) {
        P1CardValue[i + 1] = Cards[i];
//        cout << Cards[i] << " ";
        if (Cards[i] == 41) first_player = 0;
    }
//    cout << endl;

    for (int i = 13; i < 26; i++) {
        P2CardValue[i - 12] = Cards[i];
//        cout << Cards[i] << " ";
        if (Cards[i] == 41) first_player = 1;
    }
//    cout << endl;

    for (int i = 26; i < 39; i++) {
        P3CardValue[i - 25] = Cards[i];
//        cout << Cards[i] << " ";
        if (Cards[i] == 41) first_player = 2;
    }
//    cout << endl;

    for (int i = 39; i < 52; i++) {
        P4CardValue[i - 38] = Cards[i];
//       cout << Cards[i] << " ";
        if (Cards[i] == 41) first_player = 3;
    }
//    cout << endl;

    next_player = first_player;
    return first_player;
};
//发牌，同时获取一局游戏中率先出牌的玩家，根据规则，由拿到梅花二的玩家率先出牌
int HeartGame::SendCard(int i)
{
    int retVal = 0;
    switch (i) {
    case 0:
        retVal = send(Player[i].client, P1CardValue, strlen(P1CardValue) + sizeof(char), 0);
        break;
    case 1:
        retVal = send(Player[i].client, P2CardValue, strlen(P1CardValue) + sizeof(char), 0);
        break;
    case 2:
        retVal = send(Player[i].client, P3CardValue, strlen(P1CardValue) + sizeof(char), 0);
        break;
    case 3:
        retVal = send(Player[i].client, P4CardValue, strlen(P1CardValue) + sizeof(char), 0);
        break;
    default:break;
    }
    if (retVal == SOCKET_ERROR) {
        printf("send faild!\n");
    }

    return 0;
}



//确定下一轮出牌的玩家，计算玩家得分 
int HeartGame::GetMaxPlayer(int card[])
{   //确定第一个出牌的玩家 
    vector<int> CardNum;
    int color[4];
    int point = 0;
    int first = next_player;
    //确定花色
    color[0] = static_cast<int> (ceil(card[0] / 13));
    color[1] = static_cast<int> (ceil(card[1] / 13));
    color[2] = static_cast<int> (ceil(card[2] / 13));
    color[3] = static_cast<int> (ceil(card[3] / 13));
    //确定下一轮出牌的玩家 
    for (int i = 0; i < MaxPlayer; i++) {
        if (color[i] == color[0]) {
            CardNum.push_back(card[i]);
        }
        else {
            CardNum.push_back(0);
        }
    }
    auto biggest = max_element(begin(CardNum), end(CardNum));
    auto smallest = min_element(begin(CardNum), end(CardNum));
    int Max;
    if ((*smallest % 13) == 1) {                         //判断最大值是否为A 
        Max = distance(begin(CardNum), smallest);
    }
    else {
        Max = distance(begin(CardNum), biggest);
    }
    next_player = (first + Max) % 4;    //下轮出牌玩家 
    //计算玩家得分 
    for (int i = 0; i < MaxPlayer; i++) {
        if (color[i] == 1) {          //红桃记1分 
            point++;
        }
        else if (card[i] == 38) {
            point += 13;              //黑桃Q记13分
        }
    }
    GamePoint[first] += point;
    return next_player;

}
//更新当前分数 
int* HeartGame::PointCount()
{
    return GamePoint;
}

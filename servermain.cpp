// server.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Winsock2.h> //Socket�ĺ������á�
#include <windows.h>
#include <iostream>
#include "heartgame.h"

using namespace std;

#define BUF_SIZE 100  //  ��������С
#define player_num 5 //������Ϸ������
extern player Player[player_num]; //�����ĸ����
HeartGame hg;

#pragma comment (lib, "ws2_32")     // ʹ��WINSOCK2.Hʱ������Ҫ���ļ�WS2_32.LIB
//����Ϊȫ�ֱ���������ÿ���߳����������¸�ֵΪ0
int cardcount = 0;
int roundcount = 0;
int roundcard[4];
DWORD WINAPI Rcv(LPVOID lpParam)
{
    SOCKET sClient = *(SOCKET*)lpParam;
    int retVal;
    char bufRecv[BUF_SIZE];
    memset(bufRecv, 0, sizeof(bufRecv));


    while (1)
    {
        retVal = recv(sClient, bufRecv, BUF_SIZE, 0);
        if (retVal == SOCKET_ERROR) {
            printf("recive faild!\n");
            break;
        }
        else {
            printf("�յ��ͻ�����Ϣ��%s\n", bufRecv);
            switch (bufRecv[0]) {
            case 'G':
                //all clients are ready, start game
                break;
            case 'S':
                //cardcount :wait for all four players to  play in this round
                int curplayer = bufRecv[1] - '0';
                int i;
                for (i = 0;i < 4;i++) {
                    //���յ��ĳ�����Ϣת��������û�е����ƻ�������

                    if (i != curplayer) {
                        //char bufSend[BUF_SIZE];
                        if (cardcount == 0) bufRecv[4] = 1;
                        else bufRecv[4] = 0;
                    
                        int retVal = send(Player[i].client, bufRecv, strlen(bufRecv) + sizeof(char), 0);
                        if (retVal == SOCKET_ERROR) {
                            printf("send faild!\n");
                            break;
                        }
                    }
                }
                roundcard[cardcount] = (bufRecv[2] - '0') * 10 + (bufRecv[3] - '0');
                cardcount = (cardcount+1) % 4;
                int next_player;
                if (cardcount == 0) {
                    roundcount++;
                    //judge the winner for this round and calculate socre for each player
                    //round++
                    cout << "roundcount " <<roundcount<< endl;
                    //send hide flag
                    for (i = 0;i < 4;i++) {
                        int retVal;
                        char bufSend[BUF_SIZE];
                        sprintf(bufSend, "H0");                  
                        retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
                        if (retVal == SOCKET_ERROR) {
                            printf("send faild!\n");
                            break;
                        }
                    }
                    // find next player
                    next_player = hg.GetMaxPlayer(roundcard);

                    //count current score
                    int *score = hg.PointCount();
                    int i ,flag = 0,winner;
                    int max = 0;
                    for (i = 0;i<4; i++) {
                        cout << score[i] << endl;
                        if (score[i] >= max) {
                            winner = i;
                        }
                        //if one player 's score is more than 100, set end game flag
                        if (score[i] >= 100) flag = 1;
                    }
                 //   cout << "endl" << endl;
                    int retVal;
                    char bufSend[BUF_SIZE];
                    if (flag||roundcount==13) {
                        //send finish signal
                        sprintf(bufSend, "W%d", winner);
                        for (int i = 0;i < 4;i++) {
                            retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
                            if (retVal == SOCKET_ERROR) {
                                printf("send faild!\n");
                                break;
                            }
                        }
                    }
                    else {
                        //send score info
                        int i = 0;
                        for (;i < 4;i++) {
                            sprintf(bufSend, "F%03d%03d%03d%03d", score[0], score[1], score[2], score[3]);
                            retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
                            if (retVal == SOCKET_ERROR) {
                                printf("send faild!\n");
                                break;
                            }
                        }
                        //game continue
                        cout << next_player << "is next"<<endl;
                        goto nextround;
                    }        
                }
                else {          
                    next_player = (curplayer + 1)%4;
                    cout << "next"<<next_player << endl;
                    nextround:
                    for (i = 0;i < 4;i++) {
                        int retVal;
                        char bufSend[BUF_SIZE];
                        if (i == next_player) {
                            sprintf(bufSend, "T1");
                        }
                        else {
                            sprintf(bufSend, "T0");
                        }
                        retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
                        if (retVal == SOCKET_ERROR) {
                            printf("send faild!\n");
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return 0;
}

DWORD WINAPI Snd(LPVOID lpParam)
{
    SOCKET sClient = *(SOCKET*)lpParam;
    int retVal;
    char bufSend[BUF_SIZE];
    memset(bufSend, 0, sizeof(bufSend));
    while (1)
    {
      //  gets(bufSend);
        cin >> bufSend;
        retVal = send(sClient, bufSend, strlen(bufSend) + sizeof(char), 0);
        if (retVal == SOCKET_ERROR) {
            printf("send faild!\n");
            break;
        }
    }
    return 0;
}



int main(int argc, char* argv[])
{
    // ��ʼ���׽��ֶ�̬��
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("winsock load faild!\n");
        return 1;
    }
    else {
        printf("winsock load successed!\n");
    }

    //  ����������׽���
    SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sServer == INVALID_SOCKET) {
        printf("socket faild!\n");
        WSACleanup();
        return -1;
    }
    else {
        printf("socket initalize successed!\n");
    }

    //  ����˵�ַ
    sockaddr_in addrServ;

    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(9999);
    addrServ.sin_addr.s_addr = htonl(INADDR_ANY);

    //  ���׽���
    if (bind(sServer, (const struct sockaddr*) & addrServ, sizeof(addrServ)) == SOCKET_ERROR) {
        printf("bind faild!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }
    else {
        printf("bind successed!\n");
    }

    printf("Server is On IP:[%s],port:[%d]\n", inet_ntoa(addrServ.sin_addr), ntohs(addrServ.sin_port));

    //  �����׽���  ���ֱ�ʾ����ܼ����ͻ�����
    if (listen(sServer, 5) == SOCKET_ERROR) {
        printf("listen faild!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    sockaddr_in addrClient;
    int addrClientLen = sizeof(addrClient);

    int i = 0;
    int retVal;

   
    int first_player = hg.GetCards();
    for (i;i < player_num;i++) {
        Player[i].client = accept(sServer, (sockaddr FAR*) & addrClient, &addrClientLen);
        if (Player[i].client == INVALID_SOCKET) {
            printf("accept faild!\n");
            closesocket(sServer);
            WSACleanup();
            return -1;
            //����ʧ�ܺ�i-1�����µȴ����ӣ���֤���ĸ��û�����
        }
        //��ӡ�����ӵ��������˵�ip��ַ�Ͷ˿�
        printf("accepted client IP:[%s],port:[%d]\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
        
        char bufSend[BUF_SIZE];
        sprintf(bufSend, "P%d", i);
        retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
          if (retVal == SOCKET_ERROR) {
              printf("send faild!\n");
          }
       
         //�ͻ������ӵ�������ʱ����������ͻ��˷���P0/P1/P2/P3
    }
    
    cout << "all clients has connected" << endl;
    for (i = 0;i < 4;i++) {
        hg.SendCard(i);
    }
    
 //   vector <int> newcards = hg.GetCards();

    for (i = 0;i < player_num;i++) {
        int retVal;
        char bufSend[BUF_SIZE];
        if (i == first_player) {
            sprintf(bufSend, "T1");
        }
        else {
            sprintf(bufSend, "T0");
        }
        retVal = send(Player[i].client, bufSend, strlen(bufSend) + sizeof(char), 0);
        if (retVal == SOCKET_ERROR) {
            printf("send faild!\n");
            break;
        }
    }
    
    //while(gamenotfinish)->getcard->return score
    HANDLE hThread1,hThread2,hThread3,hThread4,hThread5;
    DWORD dwThreadId1, dwThreadId2, dwThreadId3, dwThreadId4, dwThreadId5;

    hThread1 = ::CreateThread(NULL, NULL, Snd, (LPVOID*)&Player[0].client, 0, &dwThreadId1);
    //�����ĸ��̼߳������Բ�ͬ�ͻ��˵ķ�����Ϣ
    hThread2 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&Player[0].client, 0, &dwThreadId2);
    hThread3 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&Player[1].client, 0, &dwThreadId3);
    hThread4 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&Player[2].client, 0, &dwThreadId4);
    hThread5 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&Player[3].client, 0, &dwThreadId5);

    ::WaitForSingleObject(hThread1, INFINITE);
    ::WaitForSingleObject(hThread2, INFINITE);
    ::WaitForSingleObject(hThread3, INFINITE);
    ::WaitForSingleObject(hThread4, INFINITE);
    ::WaitForSingleObject(hThread5, INFINITE);
    ::CloseHandle(hThread1);
    ::CloseHandle(hThread2);
    ::CloseHandle(hThread3);
    ::CloseHandle(hThread4);
    ::CloseHandle(hThread5);

   // todo: close 0-3
    closesocket(Player[0].client);
    WSACleanup(); // ��Դ�ͷ�

    return 0;
}
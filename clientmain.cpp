#include <stdio.h>
#include <Winsock2.h> //Socket�ĺ������á�
#include <windows.h>
#include <iostream>
#include <vector>


using namespace std;
#define BUF_SIZE 6400

#pragma comment (lib, "ws2_32")     // ʹ��WINSOCK2.Hʱ������Ҫ���ļ�WS2_32.LIB

int NumPlayer; //���������Ǽ���

bool Myturn = false; //�Ƿ����
vector<int> mycard;
DWORD WINAPI Rcv(LPVOID lpParam)
{
    SOCKET sHost = *(SOCKET*)lpParam;
    int retVal;
    char bufRecv[BUF_SIZE];
    memset(bufRecv, 0, sizeof(bufRecv));
    while (1)
    {
        retVal = recv(sHost, bufRecv, BUF_SIZE, 0);
        if (retVal == SOCKET_ERROR) {
            printf("recive faild!\n");
            break;
        }
        else {
         //   printf("�յ���������Ϣ��%s\n", bufRecv);
            int len = sizeof(bufRecv);
            int i = 0;
            switch (bufRecv[0])
            {
            case 'P':
                //����Լ���player��
                NumPlayer = bufRecv[1]-'0';
                cout << NumPlayer << endl;
                break;
            case 'C':
                //����Լ��ĳ�ʼ��     
                for (i = 1;i < 14;i++) {
                    mycard.push_back(bufRecv[i]);
                    cout << mycard[i - 1]<<" ";
                }
                break;
            case 'T':
                //�Ƿ����Լ��ĳ���˳��
                if (bufRecv[1] == '1') {
                    Myturn = true;
            //        hg.playcard();
                }
                else {
                    Myturn = false;
                }

                break;
            case 'F':
                if (bufRecv[1] == '1') {
                   //win
                }
                else {
                    //lose
                }
            default:
                break;
            }
        }
    }
    return 0;
}

DWORD WINAPI Snd(LPVOID lpParam)
{
    SOCKET sHost = *(SOCKET*)lpParam;
    int retVal;
    char bufSend[BUF_SIZE];
    memset(bufSend, 0, sizeof(bufSend));
    while (1)
    {
        cin>>bufSend;
        retVal = send(sHost, bufSend, strlen(bufSend) + sizeof(char), 0);
        if (retVal == SOCKET_ERROR) {
            printf("send faild!\n");
            break;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock load faild!\n");
        return 1;
    }

    //  �������׽���
    SOCKET sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sHost == INVALID_SOCKET) {
        printf("socket faild!\n");
        WSACleanup();
        return -1;
    }

    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    //  ע��   ���ѿͻ��˳��򷢵����˵ĵ���ʱ �˴�IP���Ϊ���������ڵ��Ե�IP
    servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(9999);

    //  ���ӷ�����
    if (connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("connect faild!\n");
        closesocket(sHost);
        WSACleanup();
        return -1;
    }
    printf("���ӵ������� IP:[%s],port:[%d]\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

    HANDLE hThread1, hThread2;
    DWORD dwThreadId1, dwThreadId2;

    hThread1 = ::CreateThread(NULL, NULL, Snd, (LPVOID)&sHost, 0, &dwThreadId1);
    hThread2 = ::CreateThread(NULL, NULL, Rcv, (LPVOID)&sHost, 0, &dwThreadId2);

    ::WaitForSingleObject(hThread1, INFINITE);
    ::WaitForSingleObject(hThread2, INFINITE);
    ::CloseHandle(hThread1);
    ::CloseHandle(hThread2);


    closesocket(sHost);
    WSACleanup();
    return 0;
}
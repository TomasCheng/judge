/*
  	  �������

	ҹɫ��˷�����
	�Ӳ�֪��
	���������
	ɪ���������ᣬ
	ֻ���㣬
	��ϸ˵��
	�����ߡ�

            By Jungle Wei.

*/
#include <iostream>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
//#include <thread>
#include <string>
//#include <mutex>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <queue>

#include "product\include\pdt_common_inc.h"

using namespace std;

vector <APP_INFO_S*> g_vector_appInfo;

int RegistAppInfo(APP_INFO_S *pstAppInfo)
{
	int ret = OS_OK;

	g_vector_appInfo.push_back(pstAppInfo);

	if (NULL != pstAppInfo->pfInitFunction)
	{
		ret = pstAppInfo->pfInitFunction();
		if (OS_OK != ret)
		{
			pdt_debug_print("%s Task init failed...", pstAppInfo->taskName);
			pdt_debug_print("%s Task RegistAppInfo failed...", pstAppInfo->taskName);
			return OS_ERR;
		}

		pdt_debug_print("%s Task init ok...", pstAppInfo->taskName);
	}

	pdt_debug_print("%s Task RegistAppInfo ok...", pstAppInfo->taskName);

	return OS_OK;

}

SOCKET sListen_tel;

void telnet_thread()
{

}

void init_telnet()
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return ;
	// �����׽���
	sListen_tel = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen_tel == INVALID_SOCKET)
	{
		printf("create socket error");
		return ;
	}
	// ��sockaddr_in�ṹ��װ���ַ��Ϣ
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(23);	/* htons���� ���������޷��Ŷ�������ת���������ֽ�˳��  */
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	int ret=0;
	if (bind(sListen_tel,(LPSOCKADDR)&sin,sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind failed:%d.",WSAGetLastError());
		Sleep(100);
		return ;
	}

	//�������״̬
	if (listen(sListen_tel,20) == SOCKET_ERROR)
	{
		printf("listen failed:%d.",WSAGetLastError());
		Sleep(100);
		return;
	}


	// ѭ�����ܿͻ�����������
	sockaddr_in remoteAddr;
	SOCKET sClient;
	//��ʼ���ͻ���ַ����
	int nAddrLen = sizeof(remoteAddr);
	char rev_buf[256] = {0};

	sClient = accept(sListen_tel, (SOCKADDR*)&remoteAddr, &nAddrLen);
	if(sClient == INVALID_SOCKET){
		printf("Accept() Error");
		return ;
	}

	char buffer[4094] = {0};
    sprintf(buffer, "Recv telnet request form: %s:%d\n", inet_ntoa(remoteAddr.sin_addr), ntohs(
            remoteAddr.sin_port));
	printf("%s\r\n",buffer);

	memset(rev_buf, 0, sizeof(rev_buf));
	while (recv(sClient,(char*)rev_buf,sizeof(rev_buf),0)>=0)
	{

		printf("%s", rev_buf);

		extern void cmd_resolve_enter_ex(char* buf);
		cmd_resolve_enter_ex(rev_buf);

		//send(sClient, rev_buf, sizeof(rev_buf), 0);
		memset(rev_buf, 0, sizeof(rev_buf));
	}
	Sleep(1);


	closesocket(sClient);

}

int main()
{
	pdt_debug_print("OS Main-task Running...");

	extern void Cmd_RegAppInfo();
	Cmd_RegAppInfo();

	extern void Debug_RegAppInfo();
	Debug_RegAppInfo();

	extern void Judge_RegAppInfo();
	Judge_RegAppInfo();


	for (vector<int>::size_type ix = 0; ix < g_vector_appInfo.size(); ++ix)
	{
		if (NULL != g_vector_appInfo[ix]->pfTaskMain)
		{
			_beginthread(g_vector_appInfo[ix]->pfTaskMain,0,NULL);

			pdt_debug_print("%s Task running ok...", g_vector_appInfo[ix]->taskName);
		}
	}

	pdt_debug_print("OS Main-task init ok...");

	//init_telnet();

	/* ѭ����ȡ��Ϣ���� */
	for ( ; ; )
	{
		RunDelay(1);
	}


	return 0;
}

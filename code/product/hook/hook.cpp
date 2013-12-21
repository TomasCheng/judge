#include <windows.h>
#include <shellapi.h>
#include <winsock2.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib,"ws2_32")

#define BOOL_OK 1
#define BOOL_ERR 1

/* Hook ע���API���� */
#define PROC_MAX 15

DWORD* lpAddr[PROC_MAX];
PROC OldProc[PROC_MAX];

typedef struct
{
	char szCalleeModName[MAX_PATH];
	PROC oldFunc;
	PROC newFunc;
} API_FUNC_ID;

int APIHOOK_TRACE(char *szFuncName)
{
	MessageBox(NULL, szFuncName, "API HOOK", 0);
	//puts(szFuncName);
	return 0;
}

/*
�����߳����
*/
BOOL  __stdcall MyGetCurrentProcess( VOID )
{
	APIHOOK_TRACE("GetCurrentProcess");
	return 0;
}

BOOL  __stdcall  MyCreateProcess(LPCTSTR lApplicationName,
LPTSTR lpCommandLine,
LPSECURITY_ATTRIBUTES lpProcessAttributes,
LPSECURITY_ATTRIBUTES lpThreadAttributes,
BOOL bInheritHandles,
DWORD dwCreationFlags,
LPVOID lpEnvironment,
LPCTSTR lpCurrentDirectory,
LPSTARTUPINFO lpStartupInfo,
LPPROCESS_INFORMATION lpProcessInformation )
{
	APIHOOK_TRACE("CreateProcess");
	return 0;
}

BOOL  __stdcall MyCreateThread( LPSECURITY_ATTRIBUTES lpThreadAttributes, DWORD dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId )
{
	APIHOOK_TRACE("CreateThread");
	return 0;
}

/*
�ļ����
*/
BOOL  __stdcall MyCreateFileA(
LPCWSTR lpFileName,
DWORD dwDesiredAccess,
DWORD dwShareMode,
LPSECURITY_ATTRIBUTES lpSecurityAttributes,
DWORD dwCreationDisposition,
DWORD dwFlagsAndAttributes,
HANDLE hTemplateFile)
{
	APIHOOK_TRACE("CreateFileA");
	return 0;
}

BOOL  __stdcall MyCreateFileW(
LPCWSTR lpFileName,
DWORD dwDesiredAccess,
DWORD dwShareMode,
LPSECURITY_ATTRIBUTES lpSecurityAttributes,
DWORD dwCreationDisposition,
DWORD dwFlagsAndAttributes,
HANDLE hTemplateFile)
{
	APIHOOK_TRACE("CreateFileW");
	return 0;
}

BOOL  __stdcall MyDeleteFileA(LPCSTR lpFileName)
{
	APIHOOK_TRACE("DeleteFileA");
	return 0;
}

BOOL  __stdcall MyDeleteFileW(LPCSTR lpFileName)
{
	APIHOOK_TRACE("DeleteFileW");
	return 0;
}

/*
CMD���
*/
BOOL  __stdcall MyShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	APIHOOK_TRACE("ShellExecuteA");
	return 0;
}

BOOL  __stdcall MyShellExecuteW(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	APIHOOK_TRACE("ShellExecuteW");
	return 0;
}

BOOL  __stdcall MyWinExec(LPCSTR lpCmdLine, UINT uCmdShow)
{
	APIHOOK_TRACE("WinExec");
	return 0;
}

/*
�������
*/
BOOL  __stdcall MyMessageBoxA(HWND hWnd , LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	APIHOOK_TRACE("MessageBoxA");
	return 0;
}

BOOL  __stdcall MyMessageBoxW(HWND hWnd , LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	APIHOOK_TRACE("MessageBoxW");
	return 0;
}

/*
Socket���
*/
BOOL  __stdcall MyWSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData)
{
	APIHOOK_TRACE("WSAStartup");
	return 0;
}

BOOL  __stdcall Myaccept(SOCKET s, struct sockaddr FAR *addr, int FAR *addrlen)
{
	APIHOOK_TRACE("accept");
	return 0;
}

BOOL  __stdcall Mysocket(int af, int type, int protocol)
{
	APIHOOK_TRACE("socket");
	return 0;
}

const API_FUNC_ID MANDATORY_API_FUNCS[] =
{
	{"Kernel32.dll", (PROC)GetCurrentProcess, (PROC)MyGetCurrentProcess},
	{"Kernel32.dll", (PROC)CreateProcess, (PROC)MyCreateProcess},
	{"Kernel32.dll", (PROC)CreateThread, (PROC)MyCreateThread},

	//3

	{"Kernel32.dll", (PROC)CreateFileA, (PROC)MyCreateFileA},
	{"Kernel32.dll", (PROC)CreateFileW, (PROC)MyCreateFileW},
	{"Kernel32.dll", (PROC)DeleteFileA, (PROC)MyDeleteFileA},
	{"Kernel32.dll", (PROC)DeleteFileW, (PROC)MyDeleteFileW},
	{"Kernel32.dll", (PROC)WinExec,        (PROC)MyWinExec},

	{"shell32.dll", (PROC)ShellExecuteA, (PROC)MyShellExecuteA},
	{"shell32.dll", (PROC)ShellExecuteW, (PROC)MyShellExecuteW},

	//10

	{"User32.dll", (PROC)MessageBoxA, (PROC)MyMessageBoxA},
	{"User32.dll", (PROC)MessageBoxW, (PROC)MyMessageBoxW},
	//12

	{"Ws2_32.dll", (PROC)WSAStartup, (PROC)MyWSAStartup},
	{"Ws2_32.dll", (PROC)accept, (PROC)Myaccept},
	{"Ws2_32.dll", (PROC)socket, (PROC)Mysocket},
	//15
};

int  ApiHook(char *DllName,//DLL�ļ���
				PROC OldFunAddr,//ҪHOOK�ĺ�����ַ
				PROC NewFunAddr,//���ǹ���ĺ�����ַ
				int index
)
{
	//�õ���������ģ�����ַ
	HMODULE lpBase = GetModuleHandle(NULL);
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	//IMAGE_IMPORT_BY_NAME *ImportName;
	//��λ��DOSͷ
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	//��λ��PEͷ
	ntHeader=(IMAGE_NT_HEADERS32*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//��λ�������
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//ѭ������IMAGE_IMPORT_DESCRIPTOR��������
	while(pImportDesc->FirstThunk)
	{
		//�õ�DLL�ļ���
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//�Ƚϵõ���DLL�ļ����Ƿ��ҪHOOK�������ڵ�DLL��ͬ
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
			break;
		}
		pImportDesc++;
	}
	//��λ��FirstThunk����ָ���IMAGE_THUNK_DATA����ʱ����ṹ�Ѿ��Ǻ�����ڵ��ַ��
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)
	((BYTE*)lpBase + pImportDesc->FirstThunk);
	//�����ⲿ��IAT��
	while(pThunk->u1.Function)
	{
		lpAddr[index] = (DWORD*)&(pThunk->u1.Function);
		//�ȽϺ�����ַ�Ƿ���ͬ
		if(*(lpAddr[index]) == (DWORD)OldFunAddr)
		{
			DWORD dwOldProtect;
			//�޸��ڴ��������
			VirtualProtect(lpAddr[index], sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API��������ڵ��ַ�ĳ����ǹ���ĺ����ĵ�ַ
			if (0 == WriteProcessMemory(GetCurrentProcess(),lpAddr[index], &NewFunAddr, sizeof(DWORD), NULL))
			{
				return BOOL_ERR;
			}
		}
		pThunk++;
	}
return BOOL_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule,
						DWORD  ul_reason_for_call,
						LPVOID lpReserved)
{
	int iLoop = 0;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		//�õ�������ַ
		for (iLoop = 0; iLoop < PROC_MAX; ++iLoop)
		{
			//��λ���޸�IAT��
			ApiHook((char*)MANDATORY_API_FUNCS[iLoop].szCalleeModName,
			MANDATORY_API_FUNCS[iLoop].oldFunc,
			MANDATORY_API_FUNCS[iLoop].newFunc,
			iLoop);
		}

		break;
		case DLL_PROCESS_DETACH:
		//�ָ�IAT����API��������ڵ��ַ
		for (iLoop = 0; iLoop < PROC_MAX; ++iLoop)
		{
			//��λ���޸�IAT��
			WriteProcessMemory(GetCurrentProcess(), lpAddr[iLoop], (void*)&MANDATORY_API_FUNCS[iLoop].oldFunc, sizeof(DWORD), NULL);
		}

		break;
	}
	return TRUE;
}

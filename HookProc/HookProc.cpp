// HookProc.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"

#include<stdio.h>
#include<process.h>
#include"HookProc.h"

#pragma comment(lib,"user32.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )//���ش���

#define WH_KEYBOARD_LL 13
#define PROCSIZE 6

HHOOK MyHook;                  //������SetWindowsHookEx���صľɵĹ���


CString strProc[PROCSIZE] = { _T("firefox"), _T("studio"), _T("360se"), _T("MATLAB"), _T("explorer"),_T("devenv")};


//������
int main(int argc, _TCHAR* argv[])
{
	//��װ����
	MyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)&KeyboardProc, GetModuleHandle(NULL), NULL);

	//printf("�������������");
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != -1)                   //��Ϣѭ��
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};

	UnhookWindowsHookEx(MyHook);
	return 0;
}



//�ص����������ڴ���ػ�İ�����Ϣ
int CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	UINT fuModifiers = (UINT)LOWORD(lParam);  // key-modifier flags 
	UINT uVirtKey = (UINT)HIWORD(lParam);     // virtual-key code 


	if (code >= HC_ACTION && wParam == WM_KEYDOWN) //�м�����
	{
		DWORD vk_code = ((KBDLLHOOKSTRUCT2*)lParam)->vkCode;
		//DWORD vk_code = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
		if ((vk_code == 0x5b && ((GetAsyncKeyState(VK_LCONTROL) & 0x8000))))   // CTRL=0X43��LWIN=0X5B��DEL=0X2E ��ֵ
		{
			//printf("�ɹ������� CTRL+WIN ��ϼ�\n");
			KillProcess(strProc);
		}
		//printf("lParam = %d code = %d HC_ACTION = %d WM_KEYDOWN=%d wParam = %d vk_code = %d\n", lParam, code, HC_ACTION, WM_KEYDOWN, wParam, vk_code);
		//BYTE ks[256];
		//GetKeyboardState(ks);
		//WORD w;
		//ToAscii(vk_code, 0, ks, &w, 0);
		//char ch = char(w);
		//log(ch);
		//printf("%d ",vk_code);         //���������Ϣ��ע�����ﰴ�º͵��𶼻����
	}

	//if ((GetAsyncKeyState(VK_MBUTTON) & 0x8000))
	//	KillProcess(strProc);

	return CallNextHookEx(MyHook, code, wParam, lParam);  //����Ϣ��������������ҪӰ�����
}

/*
//��¼���ļ�
int log(char vkcode){
	FILE *fl;
	fl = fopen("log.txt", "a+");
	if (vkcode == 13)
	fwrite("\r\n", 1, 2, fl);//ע��˴� count=2
	else
	fwrite(&vkcode, sizeof(char), 1, fl);//�Ѱ����ַ� ��¼���ļ�
	//printf("write ok\n");
	fclose(fl);
	return 0;
}
*/

/*
�ɽ��е��ԣ��鿴���̵�����
����Ҫ�رս��̵ĵط�����KillProcess(_T("Ҫ�رյĽ��̵�����"));���ɡ�
*/

void KillProcess(CString sExeName[])
{

	//CreateToolhelp32Snapshot()�õ�ϵͳ���̵�һ������
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if (hSnapShot == 0)
		return;
	PROCESSENTRY32 thePE;
	thePE.dwSize = sizeof(PROCESSENTRY32);


	//�����������еĵ�һ��ϵͳ����
	bool Status = (BOOL)Process32First(hSnapShot, &thePE);

	bool bHaveFlag = false; //�ҵ�������״̬
	DWORD ProcessID = 0;
	CString sTemp;

	while (Status)
	{
		//�����������е���һ��ϵͳ����  
		Status = (BOOL)Process32Next(hSnapShot, &thePE);

		//�ҵ���Ӧ�Ľ��� **.exe
		CString sFindName = thePE.szExeFile;//�����еĽ�����

		for (int i = 0; i < PROCSIZE; i++)
		{
			sTemp = sExeName[i].Mid(0, sFindName.GetLength() + 1);//Ѱ��Ҫ�رյĽ�����
			if (sFindName.Find(sTemp) != -1)
			{
				bHaveFlag = true;
				ProcessID = thePE.th32ProcessID;

				//����ָ���Ľ��� ProcessID
				TerminateProcess(OpenProcess(PROCESS_TERMINATE || PROCESS_QUERY_INFORMATION, false, ProcessID), 0);
			//	if (!TerminateProcess(OpenProcess
			//		(PROCESS_TERMINATE || PROCESS_QUERY_INFORMATION, false, ProcessID), 0))

			//	{
			//		printf_s("�޷���ָֹ���Ľ��̣�");
			//	}
			//	else
			//	{
			//		printf_s("���̽�����");
			//		break;
			//	}
			}
		}
	}
	CloseHandle(hSnapShot);
}


//ԭ������https://blog.csdn.net/laiyinping/article/details/39493457

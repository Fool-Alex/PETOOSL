#pragma once
#include "PETools.h"

void InitListView(HWND hwndDlg);//��ʼ��ListView
void ShowInfo(LPWSTR string);//�����Ϣ��ListView
DWORD Align(DWORD Num, DWORD Ali);//���ض�����ֵ
BOOL MeneryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPSTR lpszFile);//���ڴ��е�����д��Ӳ����
VOID AddSection(LPVOID pSourceBuffer, DWORD SourceSize, LPVOID pAddBuffer, DWORD AddSize);//����½�

BOOL CALLBACK AddShellDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);
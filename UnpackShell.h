#pragma once
#include "PETools.h"

BOOL CALLBACK UnpackShellDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);

void InitUnpackListView(HWND hwndDlg);//��ʼ��listControl
VOID Unpacking(LPVOID pShellBuffer, DWORD ShellSize, LPVOID* pSourceFileBuffer, DWORD* SourceFileSize);//�����Ľ���ȡԴ�ļ�
void ShowInfoUnpack(LPWSTR string);//�����Ϣ
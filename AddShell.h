#pragma once
#include "PETools.h"

void InitListView(HWND hwndDlg);//��ʼ��ListView
void ShowInfo(HWND hListShell, LPWSTR string);//�����Ϣ��ListView

BOOL CALLBACK AddShellDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);
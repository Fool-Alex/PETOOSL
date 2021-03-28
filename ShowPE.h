#pragma once
#include "PETools.h"

DWORD ReadPEFile(IN LPSTR lpszFile, IN DWORD offset, OUT LPVOID* pFileBuffer);//��ȡ�ļ�
void SetEditText(HWND hDlg, int dwEditId, TCHAR* text);//����Edit Control�ؼ�������
void TcharToChar(const TCHAR* tchar, char* _char);//TCHAR����תΪCHAR����
void ShowDirectory(HWND HwndDirectoryDlg);//��ʾ����Ŀ¼
void ShowSectionTable(HWND hListProcess);//��ȡPE������Ϣ
void GetPEHeader(IN LPSTR lpszFile);//��ȡPEͷ��Ϣ
void InitSectionTableListView(HWND hwndDlg);//��ʼ�����α��List Control
BOOL CALLBACK PEDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);
BOOL CALLBACK SectionTableDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);
BOOL CALLBACK DirectoryDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
);
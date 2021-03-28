#include "AddShell.h"

int Item = 0;

BOOL CALLBACK AddShellDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{
	//Ϊȫ�ֱ�����ֵ
	hAddShellDlg = hwndDlg;
	OPENFILENAME stOpenFile;
	switch (uMsg)
	{
	case  WM_INITDIALOG:
	{
		InitListView(hwndDlg);
		break;
	}

	case  WM_CLOSE:
	{
		EndDialog(hwndDlg, 0);
		break;
	}

	case  WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_Src:
		{
			TCHAR szPeFileExt[100] = L"*.exe;*.dll;*.scr;*.drv;*.sys";
			memset(szFileName, 0, sizeof(szFileName));
			memset(&stOpenFile, 0, sizeof(OPENFILENAME));
			stOpenFile.lStructSize = sizeof(OPENFILENAME);
			stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			stOpenFile.hwndOwner = hwndDlg;
			stOpenFile.lpstrFilter = szPeFileExt;
			stOpenFile.lpstrFile = szFileName;
			stOpenFile.nMaxFile = MAX_PATH;

			GetOpenFileName(&stOpenFile);
			if (*szFileName)
			{
				//���µĶԻ���
				//DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_PE), hwndDlg, PEDlg);
			}
			else
			{
				return TRUE;
			}
			//DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_Section), hwndDlg, SectionTableDlg);
			return TRUE;
		}
		case IDC_BUTTON_CLOSEPE:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		case IDC_BUTTON_AddShell:
		{
			//DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY), hwndDlg, DirectoryDlg);
			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}

//��ʼ��ListView
void InitListView(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListShell;
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_ShowShell���
	hListShell = GetDlgItem(hwndDlg, IDC_LIST_ShowShell);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	ShowInfo(hListShell, TEXT("��ѡ����Ҫ�ӿǵ�Դ����"));
}

//�����Ϣ��ListView
void ShowInfo(HWND hListShell, LPWSTR string)
{
	LV_ITEM vitem;
	//��ʼ��						
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	vitem.pszText = string;
	vitem.iItem = Item;
	vitem.iSubItem = 0;
	SendMessage(hListShell, LVM_INSERTITEM, 0, (DWORD)&vitem);
	Item++;
}
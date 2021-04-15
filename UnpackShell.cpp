#include "UnpackShell.h"

HWND hListUnpack;
int Count = 0;

BOOL CALLBACK UnpackShellDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{
	//Ϊȫ�ֱ�����ֵ
	hUnpackShellDlg = hwndDlg;
	HWND UnpackShellButton = GetDlgItem(hUnpackShellDlg, IDC_BUTTON_UnpackShell);
	OPENFILENAME stOpenFile;
	CHAR SrcPathA[256];
	switch (uMsg)
	{
	case  WM_INITDIALOG:
	{
		InitUnpackListView(hwndDlg);
		EnableWindow(UnpackShellButton, FALSE);
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
		case IDC_BUTTON_UnpackSrc:
		{
			TCHAR szPeFileExt[100] = L"*.exe;*.dll;*.ocx;*.sys";
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
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];
				_wsplitpath(szFileName, drive, dir, fname, ext);
				//�жϺ�׺�Ƿ�Ϊ�Ϸ��ļ�
				if (!wcscmp(ext, L".exe") || !wcscmp(ext, L".dll") || !wcscmp(ext, L".ocx") || !wcscmp(ext, L".sys"))
				{
					ShowInfoUnpack(TEXT("ѡȡ����ɹ���"));
					//��Դ������ʾ��EditControl
					SetEditText(hUnpackShellDlg, IDC_EDIT_UnpackSrc, szFileName);
					EnableWindow(UnpackShellButton, TRUE);
				}
				else
				{
					ShowInfoUnpack(TEXT("ѡȡ�ĳ����ʽ����������ѡȡ��"));
					SetEditText(hUnpackShellDlg, IDC_EDIT_UnpackSrc, TEXT(""));
					EnableWindow(UnpackShellButton, FALSE);
				}
			}
			else
			{
				ShowInfoUnpack(TEXT("ѡȡ����ʧ�ܣ������ԣ�"));
				SetEditText(hUnpackShellDlg, IDC_EDIT_UnpackSrc, TEXT(""));
				EnableWindow(UnpackShellButton, FALSE);
				return TRUE;
			}
			return TRUE;
		}
		case IDC_BUTTON_CLOSEPE:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		case IDC_BUTTON_UnpackShell:
		{
			//��ʼ�ѿ�
			ShowInfoUnpack(TEXT("��ʼ�ѿǣ����Ժ"));
			LPVOID pShellFileBuffer = NULL;
			LPVOID pSrcFileBuffer = NULL;
			DWORD SourceFileSize = 0;
			BOOL isok = false;
			// ��ȡ�ļ�
			memset(SrcPathA, 0, sizeof(SrcPathA));
			TcharToChar(szFileName, SrcPathA);
			size_t File_Size_Shell = ReadPEFile(SrcPathA, 0, &pShellFileBuffer);
			ShowInfoUnpack(TEXT("�����ѿ��У����Ժ�"));
			//��ȡ���ܺ��Դ�ļ�
			Unpacking(pShellFileBuffer, File_Size_Shell, &pSrcFileBuffer, &SourceFileSize);
			//����Դ����
			ShowInfoUnpack(TEXT("���ڽ���Դ�ļ������Ժ"));
			LPVOID pDncryptSrc = Xor(pSrcFileBuffer, SourceFileSize);
			ShowInfoUnpack(TEXT("����Դ�ļ���ϣ�"));
			char OutputFile[256] = { 0 };
			strcpy(OutputFile, pwd);
			strcat(OutputFile, "\\UnpackedFile.exe");
			isok = MeneryToFile(pDncryptSrc, SourceFileSize, OutputFile);
			if (isok)
			{
				ShowInfoUnpack(TEXT("�ѿ���ϣ��ѿǺ��ļ���Ϊ��UnpackedFile.exe"));
			}
			else
			{
				ShowInfoUnpack(TEXT("�ѿ�ʧ�ܣ������ԣ�"));
			}
			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}

//��ʼ��ListView
void InitUnpackListView(HWND hwndDlg)
{
	LV_COLUMN lv;
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_ShowShell���
	hListUnpack = GetDlgItem(hwndDlg, IDC_LIST_ShowUnpack);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�����Ϣ");		//�б���				
	lv.cx = 564;					//�п�
	lv.iSubItem = 0;				//�ڼ���
	ListView_InsertColumn(hListUnpack, 0, &lv);//�ú�ȼ��������SendMessage				
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	ShowInfoUnpack(TEXT("��ѡ����Ҫ�ѿǵĳ���"));
}

VOID Unpacking(LPVOID pShellBuffer, DWORD ShellSize, LPVOID* pSourceFileBuffer, DWORD* SourceFileSize)
{
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	BOOL isok = false;
	int i = 0;
	pDosHeader = (PIMAGE_DOS_HEADER)pShellBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pShellBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//��ȡ���һ���ڵĵ�ַ���ش�
	*pSourceFileBuffer = (char*)pShellBuffer + (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData;
	*SourceFileSize = (pSectionHeader + pPEHeader->NumberOfSections - 1)->Misc.VirtualSize;
	return;
}

//�����Ϣ��ListView
void ShowInfoUnpack(LPWSTR string)
{
	LV_ITEM vitem;
	//��ʼ��						
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	vitem.pszText = string;
	vitem.iItem = Count;
	vitem.iSubItem = 0;
	SendMessage(hListUnpack, LVM_INSERTITEM, 0, (DWORD)&vitem);
	Count++;
}
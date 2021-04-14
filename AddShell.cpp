#include "AddShell.h"

HWND hListShell;
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
	CHAR ShellFilePath[256] = "";
	CHAR SrcPathA[256];
	HWND AddShellButton = GetDlgItem(hAddShellDlg, IDC_BUTTON_AddShell);
	switch (uMsg)
	{
	case  WM_INITDIALOG:
	{
		InitListView(hwndDlg);
		EnableWindow(AddShellButton, FALSE);
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
					ShowInfo(TEXT("ѡȡԴ����ɹ���"));
					//��Դ������ʾ��EditControl
					SetEditText(hAddShellDlg, IDC_EDIT_Src, szFileName);
					EnableWindow(AddShellButton, TRUE);
				}
				else
				{
					ShowInfo(TEXT("ѡȡ��Դ�����ʽ����������ѡȡ��"));
					SetEditText(hAddShellDlg, IDC_EDIT_Src, TEXT(""));
					EnableWindow(AddShellButton, FALSE);
				}
			}
			else
			{
				ShowInfo(TEXT("ѡȡԴ����ʧ�ܣ������ԣ�"));
				SetEditText(hAddShellDlg, IDC_EDIT_Src, TEXT(""));
				EnableWindow(AddShellButton, FALSE);
				return TRUE;
			}
			return TRUE;
		}
		case IDC_BUTTON_CLOSEPE:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		case IDC_BUTTON_AddShell:
		{
			
			ShowInfo(TEXT("��ʼ�ӿǣ�"));
			//��ʼ�ӿ�
			LPVOID pShellFileBuffer = NULL;
			LPVOID pSrcFileBuffer = NULL;
			BOOL isok = false;
			// ��ȡԴ�ļ�
			memset(SrcPathA, 0, sizeof(SrcPathA));
			TcharToChar(szFileName, SrcPathA);
			DWORD File_Size_Src = ReadPEFile(SrcPathA, 0, &pSrcFileBuffer);
			//�ڴ��м��ܣ���ȡ��С

			//��ȡ���ܺ���ļ���С����offset
			strcpy(ShellFilePath, pwd);
			strcat(ShellFilePath, "\\shell.exe");
			DWORD File_Size_Shell = ReadPEFile(ShellFilePath, 0, &pShellFileBuffer);
			//�ӿǲ�����
			AddSection(pShellFileBuffer, pSrcFileBuffer, File_Size_Shell, File_Size_Src);
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
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_ShowShell���
	hListShell = GetDlgItem(hwndDlg, IDC_LIST_ShowShell);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�����Ϣ");		//�б���				
	lv.cx = 564;					//�п�
	lv.iSubItem = 0;				//�ڼ���
	ListView_InsertColumn(hListShell, 0, &lv);//�ú�ȼ��������SendMessage				
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	ShowInfo(TEXT("��ѡ����Ҫ�ӿǵ�Դ����"));
}

//�����Ϣ��ListView
void ShowInfo(LPWSTR string)
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

//���ض�����ֵ
DWORD Align(DWORD Num, DWORD Ali)
{
	int a = Num / Ali;
	return (a + 1) * Ali;
}


//����һ���ڵ�PE�ļ���
VOID AddSection(LPVOID pSourceBuffer, LPVOID pAddBuffer, DWORD SourceFileSize, DWORD AddFileSize)
{
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pNewSec = NULL;//�½ڱ�ṹ
	BOOL isok = false;
	int i = 0;
	pDosHeader = (PIMAGE_DOS_HEADER)pSourceBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pSourceBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//�жϽڱ���Ƿ��ж���ռ���ӽڱ�
	for (; i < 80; i++)
	{
		if (*((char*)(pSectionHeader + pPEHeader->NumberOfSections) + i) != 0)
		{
			ShowInfo(TEXT("�ڱ���޶���ռ䣬��������PEͷ������пռ䣡"));
			//����DOSͷ��NTͷ֮�����������
			memmove((char*)pSourceBuffer + 0x40, (char*)pSourceBuffer + pDosHeader->e_lfanew, (DWORD)(pSectionHeader + pPEHeader->NumberOfSections) - (DWORD)pSourceBuffer - pDosHeader->e_lfanew);
			//����pDosHeader->e_lfanew
			int x = pDosHeader->e_lfanew;
			pDosHeader->e_lfanew = 0x40;
			//���¸�ͷָ�븳ֵ
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pSourceBuffer + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			//�����Ǻ������������0
			memset(pSectionHeader + pPEHeader->NumberOfSections, 0, x - 0x40);
			break;
		}
	}
	//�����ڱ�ṹ
	pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	//��д�����ڱ������
	unsigned char arr[8] = ".Shell";
	memcpy(pNewSec->Name, arr, 8);
	pNewSec->Misc.VirtualSize = Align(AddFileSize, pOptionHeader->SectionAlignment);
	if (pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize > pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData)
	{
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize, pOptionHeader->SectionAlignment);
	}
	else {
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData, pOptionHeader->SectionAlignment);
	}
	pNewSec->SizeOfRawData = Align(AddFileSize, pOptionHeader->FileAlignment);
	pNewSec->PointerToRawData = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData;
	pNewSec->PointerToRelocations = 0;
	pNewSec->PointerToLinenumbers = 0;
	pNewSec->NumberOfRelocations = 0;
	pNewSec->NumberOfLinenumbers = 0;
	pNewSec->Characteristics = 0xF0000020;
	pOptionHeader->SizeOfImage += Align(AddFileSize, pOptionHeader->SectionAlignment);
	pPEHeader->NumberOfSections++;
	//��ӽ������
	pNewBuffer = malloc(SourceFileSize + pNewSec->SizeOfRawData);
	if (!pNewBuffer)
	{
		MessageBox(NULL, L"�½��ڴ�ʧ�ܣ�", L"����", MB_OK);
		free(pSourceBuffer);
		free(pAddBuffer);
		pNewBuffer = NULL;
		return;
	}
	memset(pNewBuffer, 0, SourceFileSize + pNewSec->SizeOfRawData);
	memcpy(pNewBuffer, pSourceBuffer, SourceFileSize);
	//�½�����λ��
	LPVOID pNewSection = NULL;
	pNewSection = (char*)pNewBuffer + pSectionHeader[pPEHeader->NumberOfSections - 2].PointerToRawData + pSectionHeader[pPEHeader->NumberOfSections - 2].SizeOfRawData;
	memcpy(pNewSection, pAddBuffer, AddFileSize);
	char OutputFile[256] = { 0 };
	strcpy(OutputFile, pwd);
	strcat(OutputFile, "\\AddShellFile.exe");
	isok = MeneryToFile(pNewBuffer, Align(SourceFileSize + AddFileSize, pOptionHeader->FileAlignment), OutputFile);
	if (isok)
	{
		ShowInfo(TEXT("�ӿ���ϣ��ӿǺ��ļ���Ϊ��AddShellFile.exe"));
	}
	else
	{
		ShowInfo(TEXT("�ӿ�ʧ�ܣ������ԣ�"));
	}
	free(pSourceBuffer);
	free(pAddBuffer);
	free(pNewBuffer);
	return;
}

//���ڴ��е�����д��Ӳ����
BOOL MeneryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPSTR lpszFile)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pMemBuffer)
	{
		MessageBox(NULL, L"������ָ����Ч��", L"����", MB_OK);
		return 0;
	}
	FILE* pFile = NULL;
	if ((pFile = fopen(lpszFile, "wb+")) == NULL)
	{
		MessageBox(NULL, L"�޷����ļ���", L"����", MB_OK);
		return 0;
	}
	fwrite(pMemBuffer, 1, size, pFile);
	fclose(pFile);
	pFile = NULL;
	return size;
}
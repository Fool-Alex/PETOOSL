#include "ShowPE.h"

//��ȡ�ļ�
DWORD ReadPEFile(IN LPSTR lpszFile, IN DWORD offset, OUT LPVOID* pFileBuffer)
{
	FILE* pFile = NULL;
	DWORD fileSize = 0;
	//���ļ�
	pFile = fopen(lpszFile, "rb");
	if (!pFile)
	{
		printf("�޷���EXE�ļ���");
		return 0;
	}
	//��ȡ�ļ���С
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile) + offset;
	fseek(pFile, 0, SEEK_SET);
	//���仺����
	*pFileBuffer = malloc(fileSize);
	memset(*pFileBuffer, 0, fileSize);
	if (!(*pFileBuffer))
	{
		printf("����ռ�ʧ�ܣ�");
		fclose(pFile);
		return 0;
	}
	//���ļ����ݶ�ȡ��������
	size_t n = fread(*pFileBuffer, fileSize - offset, 1, pFile);
	if (!n)
	{
		printf("��ȡ����ʧ�ܣ�");
		free(*pFileBuffer);
		fclose(pFile);
		return 0;
	}
	//�ر��ļ�
	fclose(pFile);
	pFile = NULL;
	return fileSize;
}

//����Edit Control�ؼ�������
void SetEditText(HWND hDlg, int dwEditId, TCHAR* text)
{
	HWND hEdit = GetDlgItem(hDlg, dwEditId);
	SetWindowText(hEdit, text);
}

//TCHAR����תΪCHAR����
void TcharToChar(const TCHAR* tchar, char* _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

//��ʾ����Ŀ¼
void ShowDirectory(HWND HwndDirectoryDlg)
{
	TCHAR text[20] = { 0 };
	char Filename[256] = { 0 };
	TcharToChar(szFileName, Filename);
	LPVOID pFileBuffer = NULL;
	ReadPEFile(Filename, 0, &pFileBuffer);
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)((DWORD)&pOptionHeader->NumberOfRvaAndSizes + 4);
	//��EDIT�ؼ�����ʾ����Ŀ¼��Ϣ
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_EXPORT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_EXPORT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_IMPORT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_IMPORT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_RESOURCE1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_RESOURCE2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_EXCEPTION1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_EXCEPTION2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_SECURITY1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_SECURITY2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_BASERELOC1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_BASERELOC2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_DEBUG1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_DEBUG2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_COPYRIGHT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_COPYRIGHT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_GLOBALPTR1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_GLOBALPTR2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_TLS1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_TLS2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_LOAD_CONFIG1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_LOAD_CONFIG2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_BOUND_IMPORT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_BOUND_IMPORT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_IAT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_IAT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_DELAY_IMPORT1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_DELAY_IMPORT2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_COM1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_COM2, text);
	pDataDirectory++;
	wsprintf(text, TEXT("%x"), pDataDirectory->VirtualAddress);
	SetEditText(hDirDlg, IDC_EDIT_SAVE1, text);
	wsprintf(text, TEXT("%x"), pDataDirectory->Size);
	SetEditText(hDirDlg, IDC_EDIT_SAVE2, text);
	free(pFileBuffer);
	return;
}

//��ȡPE������Ϣ
void ShowSectionTable(HWND hListProcess)
{
	LV_ITEM vitem;
	//��ʼ��						
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	int Item = 0;
	char text[256] = { 0 };
	TcharToChar(szFileName, text);
	LPVOID pFileBuffer = NULL;
	ReadPEFile(text, 0, &pFileBuffer);
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	for (size_t i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		char cBuffer[16] = { 0 };
		TCHAR tBuffer[16] = { 0 };
		wsprintfA(cBuffer, ("%8s"), pSectionHeader->Name); // Name������'\x00'��β��, ��ʽ���ַ�����Ҫ�޶�����
		// UNICODE�汾��Ҫ�� ���ֽ�ת�ɿ��ֽ�
		MultiByteToWideChar(CP_ACP, 0, cBuffer, -1, tBuffer, IMAGE_SIZEOF_SHORT_NAME);
		vitem.pszText = tBuffer;
		vitem.iItem = Item;
		vitem.iSubItem = 0;
		SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

		wsprintf(vitem.pszText, TEXT("%x"), pSectionHeader->VirtualAddress);
		vitem.iItem = Item;
		vitem.iSubItem = 1;
		ListView_SetItem(hListProcess, &vitem);

		wsprintf(vitem.pszText, TEXT("%x"), pSectionHeader->Misc.VirtualSize);
		vitem.iItem = Item;
		vitem.iSubItem = 2;
		ListView_SetItem(hListProcess, &vitem);

		wsprintf(vitem.pszText, TEXT("%x"), pSectionHeader->PointerToRawData);
		vitem.iItem = Item;
		vitem.iSubItem = 3;
		ListView_SetItem(hListProcess, &vitem);

		wsprintf(vitem.pszText, TEXT("%x"), pSectionHeader->SizeOfRawData);
		vitem.iItem = Item;
		vitem.iSubItem = 4;
		ListView_SetItem(hListProcess, &vitem);

		wsprintf(vitem.pszText, TEXT("%x"), pSectionHeader->Characteristics);
		vitem.iItem = Item;
		vitem.iSubItem = 5;
		ListView_SetItem(hListProcess, &vitem);

		Item++;
		pSectionHeader++;
	}
	free(pFileBuffer);
	return;
}

//��ȡPEͷ��Ϣ
void GetPEHeader(IN LPSTR lpszFile)
{
	TCHAR text[128] = { 0 };
	LPVOID pFileBuffer = NULL;
	ReadPEFile(lpszFile, 0, &pFileBuffer);
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	//����EDIT�ؼ���ʾPEͷ��Ϣ
	wsprintf(text, TEXT("%x"), pOptionHeader->AddressOfEntryPoint);
	SetEditText(hwndPEDialog, IDC_EDIT_Entry, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->ImageBase);
	SetEditText(hwndPEDialog, IDC_EDIT_ImageBase, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->SizeOfImage);
	SetEditText(hwndPEDialog, IDC_EDIT_ImageSize, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->BaseOfCode);
	SetEditText(hwndPEDialog, IDC_EDIT_BaseOfCode, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->BaseOfData);
	SetEditText(hwndPEDialog, IDC_EDIT_BaseOfData, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->SectionAlignment);
	SetEditText(hwndPEDialog, IDC_EDIT_SectionAlignment, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->FileAlignment);
	SetEditText(hwndPEDialog, IDC_EDIT_FileAlignment, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->Magic);
	SetEditText(hwndPEDialog, IDC_EDIT_Magic, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->Subsystem);
	SetEditText(hwndPEDialog, IDC_EDIT_Subsystem, text);
	wsprintf(text, TEXT("%x"), pPEHeader->NumberOfSections);
	SetEditText(hwndPEDialog, IDC_EDIT_NumberOfSections, text);
	wsprintf(text, TEXT("%x"), pPEHeader->TimeDateStamp);
	SetEditText(hwndPEDialog, IDC_EDIT_TimeDateStamp, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->SizeOfHeaders);
	SetEditText(hwndPEDialog, IDC_EDIT_SizeOfHeaders, text);
	wsprintf(text, TEXT("%x"), pPEHeader->Characteristics);
	SetEditText(hwndPEDialog, IDC_EDIT_Characteristics, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->CheckSum);
	SetEditText(hwndPEDialog, IDC_EDIT_CheckSum, text);
	wsprintf(text, TEXT("%x"), pPEHeader->SizeOfOptionalHeader);
	SetEditText(hwndPEDialog, IDC_EDIT_SizeOfOptionalHeader, text);
	wsprintf(text, TEXT("%x"), pOptionHeader->NumberOfRvaAndSizes);
	SetEditText(hwndPEDialog, IDC_EDIT_NumberOfRvaAndSizes, text);
	free(pFileBuffer);
	return;
}

//��ʼ�����α��List Control
void InitSectionTableListView(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hwndDlg, IDC_LIST_SectionTable);
	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("������");		//�б���				
	lv.cx = 104;						//�п�
	lv.iSubItem = 0;					//�ڼ���
	ListView_InsertColumn(hListProcess, 0, &lv);
	//�ڶ���								
	lv.pszText = TEXT("�ļ�ƫ��");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);
	//������								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�ļ���С");		//�б���				
	lv.cx = 100;						//�п�
	lv.iSubItem = 2;					//�ڼ���
	ListView_InsertColumn(hListProcess, 2, &lv);
	//������								
	lv.pszText = TEXT("�ڴ�ƫ��");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);
	//������								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�ڴ��С");		//�б���				
	lv.cx = 100;						//�п�
	lv.iSubItem = 4;					//�ڼ���
	ListView_InsertColumn(hListProcess, 4, &lv);
	//������								
	lv.pszText = TEXT("��������");
	lv.cx = 100;
	lv.iSubItem = 5;
	ListView_InsertColumn(hListProcess, 5, &lv);
}

 BOOL CALLBACK DirectoryDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{
	hDirDlg = hwndDlg;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		ShowDirectory(hwndDlg);
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
		case IDC_BUTTON_CLOSEDIR:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}

BOOL CALLBACK SectionTableDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{
	switch (uMsg)
	{
	case  WM_INITDIALOG:
	{
		InitSectionTableListView(hwndDlg);
		ShowSectionTable(GetDlgItem(hwndDlg, IDC_LIST_SectionTable));
		break;
	}

	case  WM_CLOSE:
	{
		EndDialog(hwndDlg, 0);
		break;
	}
	}
	return FALSE;
}


BOOL CALLBACK PEDlg(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message						
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{
	//Ϊȫ�ֱ�����ֵ
	hwndPEDialog = hwndDlg;
	switch (uMsg)
	{
	case  WM_INITDIALOG:
	{
		char text[256] = { 0 };
		TcharToChar(szFileName, text);
		GetPEHeader(text);
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
		case IDC_BUTTON_SECTION:
		{
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_Section), hwndDlg, SectionTableDlg);
			return TRUE;
		}
		case IDC_BUTTON_CLOSEPE:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		case IDC_BUTTON_DATADIR:
		{
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY), hwndDlg, DirectoryDlg);
			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}
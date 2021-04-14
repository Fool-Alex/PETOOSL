#include "EnumProcessAndMoudle.h"

//��������
void EnumProcess(HWND hListProcess)
{
	LV_ITEM vitem;
	//��ʼ��						
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	//��������
	//�������̿���
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	int Item = 0;
	if (hSnapshot != INVALID_HANDLE_VALUE && Process32First(hSnapshot, &pe))
	{
		do
		{
			vitem.pszText = pe.szExeFile;
			vitem.iItem = Item;
			vitem.iSubItem = 0;
			//ListView_InsertItem(hListProcess, &vitem);						
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);


			//��PIDת��ΪTCHAR����
			wsprintf(vitem.pszText, L"%d", pe.th32ProcessID);
			vitem.iItem = Item;
			vitem.iSubItem = 1;
			ListView_SetItem(hListProcess, &vitem);

			wsprintf(vitem.pszText, L"%d", pe.pcPriClassBase);
			vitem.iItem = Item;
			vitem.iSubItem = 2;
			ListView_SetItem(hListProcess, &vitem);

			wsprintf(vitem.pszText, L"%d", pe.cntThreads);
			vitem.iItem = Item;
			vitem.iSubItem = 3;
			ListView_SetItem(hListProcess, &vitem);

			Item++;
		} while (Process32Next(hSnapshot, &pe));
	}
	CloseHandle(hSnapshot);
}


//����ģ��
void EnumMoudle(HWND hListProcess, HWND hListModules)
{
	DWORD dwRowid;
	wchar_t szPid[0x20] = { 0 };
	LV_ITEM lv;
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	DWORD dwPid = 0;
	int Item = 0;
	//��ʼ��						
	memset(&lv, 0, sizeof(LV_ITEM));
	memset(szPid, 0, 0x20);
	//��ȡѡ����
	dwRowid = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (dwRowid == -1)
	{
		MessageBox(NULL, TEXT("��ѡ�����"), TEXT("ERROR"), MB_OK);
		return;
	}

	// �������������
	ListView_DeleteAllItems(hListModules);

	//��ȡPID
	lv.iSubItem = 1;
	lv.pszText = szPid;
	lv.cchTextMax = 0x20;
	SendMessage(hListProcess, LVM_GETITEMTEXT, (WPARAM)dwRowid, (LPARAM)&lv);

	//��PIDת��ΪDWORD
	swscanf_s(szPid, L"%d", &dwPid);

	// 1. ����һ��ģ����صĿ��վ��
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);


	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}
	MODULEENTRY32 mo;
	mo.dwSize = sizeof(MODULEENTRY32);

	// 2. ͨ��ģ����վ����ȡ��һ��ģ����Ϣ
	if (Module32First(hModuleSnap, &mo))
	{
		//3. ����ģ��
		do
		{
			lv.pszText = mo.szModule;
			lv.iItem = Item;
			lv.iSubItem = 0;
			//ListView_InsertItem(hListProcess, &vitem);						
			SendMessage(hListModules, LVM_INSERTITEM, 0, (DWORD)&lv);

			lv.pszText = mo.szExePath;
			lv.iItem = Item;
			lv.iSubItem = 1;
			ListView_SetItem(hListModules, &lv);

			Item++;
		} while (Module32Next(hModuleSnap, &mo));
	}

	CloseHandle(hModuleSnap);
	return;
}

//��ʼ�����̵�ListControl
void InitProcessListView(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("����");		//�б���				
	lv.cx = 205;					//�п�
	lv.iSubItem = 0;				//�ڼ���
	ListView_InsertColumn(hListProcess, 0, &lv);//�ú�ȼ��������SendMessage										
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	//�ڶ���								
	lv.pszText = TEXT("PID");
	lv.cx = 64;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
	//������								
	lv.pszText = TEXT("�߳����ȼ�");
	lv.cx = 110;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//������								
	lv.pszText = TEXT("�߳���");
	lv.cx = 110;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);
	EnumProcess(hListProcess);
}

//��ʼ��ģ���ListControl
void InitMoudleListView(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESS2);
	//��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("ģ������");		//�б���				
	lv.cx = 180;						//�п�
	lv.iSubItem = 0;					//�ڼ���
	ListView_InsertColumn(hListProcess, 0, &lv);//�ú�ȼ��������SendMessage										
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	//�ڶ���								
	lv.pszText = TEXT("ģ��λ��");
	lv.cx = 326;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);
}
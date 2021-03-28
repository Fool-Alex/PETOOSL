#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <malloc.h>
#include <memory.h>
#include <windows.h>
#include <winnt.h>
#include <string.h>

#define FILEPATH_IN "F:/VS_Project/Win_App/Debug/Win_App.exe"
#define FILEPATH_OUT "F:/RE/Windows/RE_test/Test2.exe"
#define SHELLCODELENGTH 0x12
#define MESSAGEBOXADDR 0x77D5050B
#define SIZEOFADDSECTION 0x1000

BYTE ShellCode[] =
{ 0x6A,00,0x6A,00,0x6A,00,0x6A,00,
	0xE8,00,00,00,00,
	0xE9,00,00,00,00 };

//���ض�����ֵ
DWORD Align(DWORD Num, DWORD Ali)
{
	int a = Num / Ali;
	return (a + 1) * Ali;
}



//����FileBuffer��ImageBuffer
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pFileBuffer)
	{
		printf("������ָ����Ч");
		return 0;
	}
	//��ʼ��dos����׼PE����ѡPE�ͽڱ�4���ṹ��ָ��
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//����ImageBuffer�ڴ�
	*pImageBuffer = malloc(pOptionHeader->SizeOfImage);
	if (!pImageBuffer)
	{
		printf("����ImageBuffer����");
		free(pImageBuffer);
		return 0;
	}
	memset(*pImageBuffer, 0, pOptionHeader->SizeOfImage);
	//��FileBuffer���Ƶ�ImageBuffer
	char* temppFileBuffer = (char*)pFileBuffer;
	char* temppImageBuffer = (char*)*pImageBuffer;
	memcpy(temppImageBuffer, temppFileBuffer, pOptionHeader->SizeOfHeaders);
	temppFileBuffer = (char*)pFileBuffer;
	temppImageBuffer = (char*)*pImageBuffer;
	for (int j = 0; j < (signed)pPEHeader->NumberOfSections; j++)
	{
		memcpy(temppImageBuffer + pSectionHeader->VirtualAddress, temppFileBuffer + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData);
		pSectionHeader++;
	}
	temppFileBuffer = NULL;
	temppImageBuffer = NULL;
	return pOptionHeader->SizeOfImage;
}

//��ImageBuffer���Ƶ�NewBuffer
DWORD CopyImageBufferToNewBuffer(IN LPVOID pImageBuffer, OUT LPVOID* pNewBuffer)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pImageBuffer)
	{
		printf("������ָ����Ч");
		return 0;
	}
	//��ʼ��dos����׼PE����ѡPE�ͽڱ�4���ṹ��ָ��
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pImageBuffer);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//����ImageBuffer�ڴ�
	//��ȡpNewBuffer�Ĵ�С
	DWORD NewBuffer_size = pSectionHeader[pPEHeader->NumberOfSections - 1].PointerToRawData;
	NewBuffer_size += pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData;
	*pNewBuffer = malloc(NewBuffer_size);
	if (!*pNewBuffer)
	{
		printf("����NewBuffer����");
		free(pImageBuffer);
		return 0;
	}
	memset(*pNewBuffer, 0, NewBuffer_size);
	//��FileBuffer���Ƶ�ImageBuffer
	char* temppImageBuffer = (char*)pImageBuffer;
	char* temppNewBuffer = (char*)*pNewBuffer;
	memcpy(temppNewBuffer, temppImageBuffer, pOptionHeader->SizeOfHeaders);
	temppImageBuffer = (char*)pImageBuffer;
	temppNewBuffer = (char*)*pNewBuffer;
	for (int j = 0; j < (signed)pPEHeader->NumberOfSections; j++)
	{
		memcpy(temppNewBuffer + pSectionHeader->PointerToRawData, temppImageBuffer + pSectionHeader->VirtualAddress, pSectionHeader->SizeOfRawData);
		pSectionHeader++;
	}
	temppImageBuffer = NULL;
	temppNewBuffer = NULL;
	return NewBuffer_size;
}

//���ڴ�ƫ��ת��Ϊ�ļ�ƫ��
DWORD RvaToFileOffset(IN LPVOID pFileBuffer, IN DWORD dwRva)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pFileBuffer)
	{
		printf("������ָ����Ч");
		return 0;
	}
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	for (int i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		DWORD dwBlockCount = pSectionHeader[i].SizeOfRawData / pOptionHeader->SectionAlignment;
		dwBlockCount += pSectionHeader[i].SizeOfRawData % pOptionHeader->SectionAlignment ? 1 : 0;
		if (dwRva >= pSectionHeader[i].VirtualAddress && dwRva < (pSectionHeader[i].VirtualAddress + (dwBlockCount * pOptionHeader->SectionAlignment)))
		{
			return (pSectionHeader[i].PointerToRawData + (dwRva - pSectionHeader[i].VirtualAddress));
		}
		else if (dwRva < pSectionHeader[0].VirtualAddress)
		{
			return dwRva;
		}
	}
	return 0;
}

//���ļ�ƫ��ת��Ϊ�ڴ�ƫ��
DWORD FoaToRva(IN LPVOID pFileBuffer, IN DWORD dwFoa)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pFileBuffer)
	{
		printf("������ָ����Ч");
		return 0;
	}
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//�ж��Ƿ�����Ч��MZ��־
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("������Ч��MZ��־\n");
		free(pFileBuffer);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	for (int i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		if (dwFoa >= pSectionHeader[i].PointerToRawData && dwFoa < (pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData))
		{
			return (pSectionHeader[i].VirtualAddress + (dwFoa - pSectionHeader[i].PointerToRawData));
		}
		else if (dwFoa < pSectionHeader[0].PointerToRawData)
		{
			return dwFoa;
		}
	}
	return 0;
}

//���ڴ��е�����д��Ӳ����
BOOL MeneryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPSTR lpszFile)
{
	//��⴫��ָ���Ƿ�Ϊ��
	if (!pMemBuffer)
	{
		printf("������ָ����Ч");
		return 0;
	}
	FILE* pFile = NULL;
	if ((pFile = fopen(lpszFile, "wb+")) == NULL)
	{
		printf("file open error\n");
		return 0;
	}
	fwrite(pMemBuffer, 1, size, pFile);
	fclose(pFile);
	pFile = NULL;
	return size;
}

//���shellcode������ο�����
VOID TestAddCodeInCodeSec()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PBYTE CodeBegin = NULL;
	BOOL isok = false;
	DWORD size = 0;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��");
		return;
	}
	//����FileBuffer��ImageBuffer
	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("����FileBuffer��ImageBufferʧ��");
		free(pFileBuffer);
		return;
	}
	//�жϴ���ο������Ƿ����㹻�ռ����ShellCode
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	if (SHELLCODELENGTH > (pSectionHeader->SizeOfRawData - pSectionHeader->Misc.VirtualSize))
	{
		printf("����ο�����û���㹻�ռ�");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	//��ʼ������ο��������shellcode
	CodeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	memcpy(CodeBegin, ShellCode, SHELLCODELENGTH);
	//��E8
	DWORD CallAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(CodeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(CodeBegin + 0x9) = CallAddr;
	//��E9
	DWORD JmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(CodeBegin + SHELLCODELENGTH) - (DWORD)pImageBuffer));
	*(PDWORD)(CodeBegin + 0xE) = JmpAddr;
	//��EntryPoint
	pOptionHeader->AddressOfEntryPoint = (DWORD)CodeBegin - (DWORD)pImageBuffer;
	//����ImageBuffer��NewBuffer
	size = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (size == 0 || !pNewBuffer)
	{
		printf("����ImageBuffer��NewBufferʧ��");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	//��NewBufferд��Ӳ��
	isok = MeneryToFile(pNewBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���");
	}
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	return;
}

//������ָ���������shellcode
VOID TestAddCodeInAnyCodeSec(int x)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PBYTE CodeBegin = NULL;
	BOOL isok = false;
	DWORD size = 0;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��");
		return;
	}
	//����FileBuffer��ImageBuffer
	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("����FileBuffer��ImageBufferʧ��");
		free(pFileBuffer);
		return;
	}
	//�жϴ���ο������Ƿ����㹻�ռ����ShellCode
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	if (x > pPEHeader->NumberOfSections || x < 1)
	{
		printf("ָ���������Ϸ�");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	pSectionHeader += (x - 1);
	if (pSectionHeader->Misc.VirtualSize > pSectionHeader->SizeOfRawData || SHELLCODELENGTH > (pSectionHeader->SizeOfRawData - pSectionHeader->Misc.VirtualSize))
	{
		printf("����ο�����û���㹻�ռ�");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	//��ʼ������ο��������shellcode
	CodeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	memcpy(CodeBegin, ShellCode, SHELLCODELENGTH);
	//��E8
	DWORD CallAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(CodeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(CodeBegin + 0x9) = CallAddr;
	//��E9
	DWORD JmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(CodeBegin + SHELLCODELENGTH) - (DWORD)pImageBuffer));
	*(PDWORD)(CodeBegin + 0xE) = JmpAddr;
	//��OEP
	pOptionHeader->AddressOfEntryPoint = (DWORD)CodeBegin - (DWORD)pImageBuffer;
	//�޸�������������Ϊ��ִ��
	DWORD a = pSectionHeader->Characteristics;
	DWORD b = (pSectionHeader - (x - 1))->Characteristics;
	pSectionHeader->Characteristics = a | b;
	//����ImageBuffer��NewBuffer
	size = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (size == 0 || !pNewBuffer)
	{
		printf("����ImageBuffer��NewBufferʧ��");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	//��NewBufferд��Ӳ��
	isok = MeneryToFile(pNewBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���");
	}
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	return;
}

//����һ���ڵ�PE�ļ���
VOID AddSection()
{
	LPVOID pFileBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	BOOL isok = false;
	DWORD size = 0;
	int i = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//�жϽڱ���Ƿ��ж���ռ���ӽڱ�
	for (; i < 80; i++)
	{
		if (*((char*)(pSectionHeader + pPEHeader->NumberOfSections) + i) != 0)
		{
			printf("�ڱ���޶���ռ䣬��������PEͷ������пռ�\n");
			//����DOSͷ��NTͷ֮�����������
			memmove((char*)pFileBuffer + 0x40, (char*)pFileBuffer + pDosHeader->e_lfanew, (DWORD)(pSectionHeader + pPEHeader->NumberOfSections) - (DWORD)pFileBuffer - pDosHeader->e_lfanew);
			//����pDosHeader->e_lfanew
			int x = pDosHeader->e_lfanew;
			pDosHeader->e_lfanew = 0x40;
			//���¸�ͷָ�븳ֵ
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			//�����Ǻ������������0
			memset(pSectionHeader + pPEHeader->NumberOfSections, 0, x - 0x40);
			break;
		}
	}
	//��ӽڱ�
	if (i == 80)
	{
		printf("�ڱ���ж���ռ䣬ֱ������½ڱ�\n");
	}
	pOptionHeader->SizeOfImage += SIZEOFADDSECTION;
	pPEHeader->NumberOfSections++;
	unsigned char arr[8] = "export";
	memcpy((pSectionHeader + pPEHeader->NumberOfSections - 1)->Name, arr, 8);
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->Misc.VirtualSize = SIZEOFADDSECTION;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->VirtualAddress = (pSectionHeader + pPEHeader->NumberOfSections - 2)->VirtualAddress + (pSectionHeader + pPEHeader->NumberOfSections - 2)->SizeOfRawData;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData = SIZEOFADDSECTION;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData = (pSectionHeader + pPEHeader->NumberOfSections - 2)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 2)->SizeOfRawData;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRelocations = 0;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToLinenumbers = 0;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->NumberOfRelocations = 0;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->NumberOfLinenumbers = 0;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->Characteristics = pSectionHeader->Characteristics;
	//��ӽ������
	memset((void*)((DWORD)pFileBuffer + (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData), 0, SIZEOFADDSECTION);
	//��FileBufferд��Ӳ��
	size += SIZEOFADDSECTION;
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	return;
}

//�������һ���ڲ���Ӵ���
VOID AddSectionToLastSec()
{
	LPVOID pFileBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	BOOL isok = false;
	DWORD size = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, SIZEOFADDSECTION, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//��Ӵ��뵽���һ���ڵĺ���
	memset((void*)((DWORD)pFileBuffer + (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData), 0, SIZEOFADDSECTION);
	//�޸Ľڱ�ʹ�С
	pOptionHeader->SizeOfImage += SIZEOFADDSECTION;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->Misc.VirtualSize += SIZEOFADDSECTION;
	(pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData += SIZEOFADDSECTION;
	//��FileBufferд��Ӳ��
	size += SIZEOFADDSECTION;
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	return;
}

//�ϲ���
VOID AddSectionToOneSec()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	BOOL isok = false;
	DWORD size = 0;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	//����FileBuffer��ImageBuffer
	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("����FileBuffer��ImageBufferʧ��");
		free(pFileBuffer);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//�޸ĵ�һ���ڵ�vs��soraw
	pSectionHeader->SizeOfRawData = pOptionHeader->SizeOfImage - pSectionHeader->VirtualAddress;
	pSectionHeader->Misc.VirtualSize = pSectionHeader->SizeOfRawData;
	for (int i = 1; i < pPEHeader->NumberOfSections; i++)
	{
		pSectionHeader->Characteristics |= (pSectionHeader + i)->Characteristics;
	}
	pPEHeader->NumberOfSections = 0x1;
	size = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (size == 0 || !pNewBuffer)
	{
		printf("����ImageBuffer��NewBufferʧ��");
		free(pFileBuffer);
		free(pImageBuffer);
		return;
	}
	//��FileBufferд��Ӳ��
	isok = MeneryToFile(pNewBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	pFileBuffer = NULL;
	pImageBuffer = NULL;
	pNewBuffer = NULL;
	return;
}

//��ӡ����Ŀ¼
VOID PrintImage_Data_Directory()
{
	LPVOID pFileBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)((DWORD)&pOptionHeader->NumberOfRvaAndSizes + 4);
	printf("------������------\n");
	printf("VirtualAddress:%x\nSize:%x\n", pDataDirectory->VirtualAddress, pDataDirectory->Size);
	printf("------������------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 1)->VirtualAddress, (pDataDirectory + 1)->Size);
	printf("------��Դ��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 2)->VirtualAddress, (pDataDirectory + 2)->Size);
	printf("------�쳣��Ϣ��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 3)->VirtualAddress, (pDataDirectory + 3)->Size);
	printf("------��ȫ֤���------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 4)->VirtualAddress, (pDataDirectory + 4)->Size);
	printf("------�ض�λ��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 5)->VirtualAddress, (pDataDirectory + 5)->Size);
	printf("------������Ϣ��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 6)->VirtualAddress, (pDataDirectory + 6)->Size);
	printf("------��Ȩ���б�------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 7)->VirtualAddress, (pDataDirectory + 7)->Size);
	printf("------ȫ��ָ���------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 8)->VirtualAddress, (pDataDirectory + 8)->Size);
	printf("------TLS��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 9)->VirtualAddress, (pDataDirectory + 9)->Size);
	printf("------�������ñ�------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 10)->VirtualAddress, (pDataDirectory + 10)->Size);
	printf("------�󶨵����------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 11)->VirtualAddress, (pDataDirectory + 11)->Size);
	printf("------IAT��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 12)->VirtualAddress, (pDataDirectory + 12)->Size);
	printf("------�ӳٵ����------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 13)->VirtualAddress, (pDataDirectory + 13)->Size);
	printf("------COM��Ϣ��------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 14)->VirtualAddress, (pDataDirectory + 14)->Size);
	printf("------������------\n");
	printf("VirtualAddress:%x\nSize:%x\n", (pDataDirectory + 15)->VirtualAddress, (pDataDirectory + 15)->Size);
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}


//ͨ����������ȡ������ַ
DWORD GetFunctionAddrByName(LPVOID pFileBuffer, LPSTR FunctionName)
{
	int** paddress_of_names = NULL;
	short* paddress_of_namesordinals = NULL;
	int* paddress_of_functions = NULL;
	unsigned int index_addr = 0;
	unsigned int AddressOfFunctions = 0;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY pDirEntryExport = NULL;
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pDirEntryExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory->VirtualAddress));
	paddress_of_names = (int**)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfNames));
	paddress_of_namesordinals = (short*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfNameOrdinals));
	paddress_of_functions = (int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfFunctions));
	*paddress_of_names = (int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, (DWORD)*paddress_of_names));
	//ƥ�������ַ����ͺ�����
	for (; index_addr < pDirEntryExport->NumberOfNames; index_addr++)
	{
		if (!strcmp((char*)(*paddress_of_names), FunctionName))
		{
			break;
		}
		(*paddress_of_names)++;
	}
	AddressOfFunctions = (DWORD) * (paddress_of_functions + *(paddress_of_namesordinals + index_addr));
	free(pFileBuffer);
	return AddressOfFunctions;
}


//ͨ��������Ż�ȡ������ַ
DWORD GetFunctionAddrByOrdinals(LPVOID pFileBuffer, int AddrNum)
{
	unsigned int AddressOfFunctions = 0;
	int* paddress_of_functions = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY pDirEntryExport = NULL;
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pDirEntryExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory->VirtualAddress));
	paddress_of_functions = (int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfFunctions));
	AddressOfFunctions = (DWORD) * (paddress_of_functions + (AddrNum - pDirEntryExport->Base));
	free(pFileBuffer);
	return AddressOfFunctions;
}

//��ӡ�ض�λ��
VOID PrintBASERELOC(LPVOID pFileBuffer)
{
	DWORD NumOfReloc = 1;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BASE_RELOCATION pBasereloc = NULL;
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[5].VirtualAddress));
	short* pRva = NULL;
	//��ӡ��������ƫ����Ϣ
	printf("��ӡ�ض�λ��\n");
	while (pBasereloc->VirtualAddress && pBasereloc->SizeOfBlock)
	{
		pRva = (short*)((char*)pBasereloc + 8);
		printf("------��%d��------\n", NumOfReloc);
		for (unsigned int i = 0; i < (pBasereloc->SizeOfBlock - 8) / 2; i++)
		{
			if ((*pRva & 0xf000) == 0x3000)
			{
				printf("��%d��	��ַ��%x\n", i + 1, (*pRva & 0xfff) + pBasereloc->VirtualAddress);
			}
			pRva++;
		}
		pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pBasereloc + pBasereloc->SizeOfBlock);
		NumOfReloc++;
	}
	free(pFileBuffer);
	return;
}

//�ƶ��������½���
VOID MovExport()
{
	LPVOID pFileBuffer = NULL;
	int** paddress_of_names = NULL;
	short* paddress_of_nameordinals = NULL;
	int* paddress_of_functions = NULL;
	BOOL isok = false;
	DWORD size = 0;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pNewSec = NULL;//�½ڱ�ṹ
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY pDirEntryExport = NULL;
	PIMAGE_EXPORT_DIRECTORY pNewDirEntryExport = NULL;
	size_t i = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, SIZEOFADDSECTION, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pDirEntryExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory->VirtualAddress));
	paddress_of_names = (int**)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfNames));
	paddress_of_nameordinals = (short*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfNameOrdinals));
	paddress_of_functions = (int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDirEntryExport->AddressOfFunctions));
	//�жϽڱ���Ƿ��ж���ռ���ӽڱ�
	for (; i < 80; i++)
	{
		if (*((char*)(pSectionHeader + pPEHeader->NumberOfSections) + i) != 0)
		{
			printf("�ڱ���޶���ռ䣬��������PEͷ������пռ�\n");
			//����DOSͷ��NTͷ֮�����������
			memmove((char*)pFileBuffer + 0x40, (char*)pFileBuffer + pDosHeader->e_lfanew, (DWORD)(pSectionHeader + pPEHeader->NumberOfSections) - (DWORD)pFileBuffer - pDosHeader->e_lfanew);
			//����pDosHeader->e_lfanew
			int x = pDosHeader->e_lfanew;
			pDosHeader->e_lfanew = 0x40;
			//���¸�ͷָ�븳ֵ
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			//�����Ǻ������������0
			memset(pSectionHeader + pPEHeader->NumberOfSections, 0, x - 0x40);
			break;
		}
	}
	//��ӽڱ�
	if (i == 80)
	{
		printf("�ڱ���ж���ռ䣬ֱ������½ڱ�\n");
	}
	//�����ڱ�ṹ
	pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	//��д�����ڱ������
	unsigned char arr[8] = ".export";
	memcpy(pNewSec->Name, arr, 8);
	pNewSec->Misc.VirtualSize = SIZEOFADDSECTION;
	if (pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize > pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData)
	{
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize, pOptionHeader->SectionAlignment);
	}
	else {
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData, pOptionHeader->SectionAlignment);
	}
	pNewSec->SizeOfRawData = SIZEOFADDSECTION;
	pNewSec->PointerToRawData = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData;
	pNewSec->PointerToRelocations = 0;
	pNewSec->PointerToLinenumbers = 0;
	pNewSec->NumberOfRelocations = 0;
	pNewSec->NumberOfLinenumbers = 0;
	pNewSec->Characteristics = 0x60000020;
	pOptionHeader->SizeOfImage += SIZEOFADDSECTION;
	pPEHeader->NumberOfSections++;
	//��¼�µĺ�����ַ���ַ
	int* Newaddr_of_functions = (int*)((DWORD)pFileBuffer + pNewSec->PointerToRawData);
	//����AddressOfFunctions���½�
	memcpy(Newaddr_of_functions, paddress_of_functions, 4 * pDirEntryExport->NumberOfFunctions);
	//��¼�µĺ�����ű��ַ
	short* Newaddr_of_nameordinals = (short*)((DWORD)Newaddr_of_functions + 4 * pDirEntryExport->NumberOfFunctions);
	//����address_of_nameordinals���½�
	memcpy(Newaddr_of_nameordinals, paddress_of_nameordinals, 2 * pDirEntryExport->NumberOfNames);
	//��¼�µĺ������Ʊ��ַ
	int** Newaddr_of_names = (int**)((DWORD)Newaddr_of_nameordinals + 2 * pDirEntryExport->NumberOfNames);
	//����address_of_name���½�
	memcpy(Newaddr_of_names, paddress_of_names, 4 * pDirEntryExport->NumberOfNames);
	//��¼�׸��������ĵ�ַ
	char* AdderessOfString = (char*)((DWORD)Newaddr_of_names + 4 * pDirEntryExport->NumberOfNames);
	//��ʼ���ƺ�����ͬʱ��д�������ĵ�ַ��
	for (size_t j = 0; j < pDirEntryExport->NumberOfNames; j++)
	{
		LPSTR name = (char*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, (DWORD)(*(paddress_of_names + j))));
		i = strlen(name) + 1;
		memcpy(AdderessOfString, name, i);
		*(Newaddr_of_names + j) = (int*)FoaToRva(pFileBuffer, (DWORD)AdderessOfString - (DWORD)pFileBuffer);
		AdderessOfString += i;
	}
	//��¼������ṹ�׵�ַ
	pNewDirEntryExport = (PIMAGE_EXPORT_DIRECTORY)AdderessOfString;
	//���Ƶ�����ṹ
	memcpy(pNewDirEntryExport, pDirEntryExport, pDataDirectory->Size);
	//�޸�IMAGE_EXPORT_DIRECTORY�ṹ
	pNewDirEntryExport->AddressOfFunctions = FoaToRva(pFileBuffer, (DWORD)Newaddr_of_functions - (DWORD)pFileBuffer);
	pNewDirEntryExport->AddressOfNameOrdinals = FoaToRva(pFileBuffer, (DWORD)Newaddr_of_nameordinals - (DWORD)pFileBuffer);
	pNewDirEntryExport->AddressOfNames = FoaToRva(pFileBuffer, (DWORD)Newaddr_of_names - (DWORD)pFileBuffer);

	//�޸�����Ŀ¼�ĵ������ַ
	pDataDirectory->VirtualAddress = FoaToRva(pFileBuffer, (DWORD)pNewDirEntryExport - (DWORD)pFileBuffer);
	//����
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}

//�ƶ��ض�λ���½���
VOID MovRelocation()
{
	LPVOID pFileBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pNewSec = NULL;//�½ڱ�ṹ
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BASE_RELOCATION pBasereloc = NULL;
	PIMAGE_BASE_RELOCATION pNewBasereloc = NULL;
	BOOL isok = false;
	DWORD size = 0;
	size_t i = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, SIZEOFADDSECTION, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[5].VirtualAddress));
	//�жϽڱ���Ƿ��ж���ռ���ӽڱ�
	for (; i < 80; i++)
	{
		if (*((char*)(pSectionHeader + pPEHeader->NumberOfSections) + i) != 0)
		{
			printf("�ڱ���޶���ռ䣬��������PEͷ������пռ�\n");
			//����DOSͷ��NTͷ֮�����������
			memmove((char*)pFileBuffer + 0x40, (char*)pFileBuffer + pDosHeader->e_lfanew, (DWORD)(pSectionHeader + pPEHeader->NumberOfSections) - (DWORD)pFileBuffer - pDosHeader->e_lfanew);
			//����pDosHeader->e_lfanew
			int x = pDosHeader->e_lfanew;
			pDosHeader->e_lfanew = 0x40;
			//���¸�ͷָ�븳ֵ
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			//�����Ǻ������������0
			memset(pSectionHeader + pPEHeader->NumberOfSections, 0, x - 0x40);
			break;
		}
	}
	//��ӽڱ�
	if (i == 80)
	{
		printf("�ڱ���ж���ռ䣬ֱ������½ڱ�\n");
	}
	//�����ڱ�ṹ
	pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	//��д�����ڱ������
	unsigned char arr[8] = ".reloca";
	memcpy(pNewSec->Name, arr, 8);
	pNewSec->Misc.VirtualSize = SIZEOFADDSECTION;
	if (pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize > pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData)
	{
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize, pOptionHeader->SectionAlignment);
	}
	else {
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData, pOptionHeader->SectionAlignment);
	}
	pNewSec->SizeOfRawData = SIZEOFADDSECTION;
	pNewSec->PointerToRawData = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData;
	pNewSec->PointerToRelocations = 0;
	pNewSec->PointerToLinenumbers = 0;
	pNewSec->NumberOfRelocations = 0;
	pNewSec->NumberOfLinenumbers = 0;
	pNewSec->Characteristics = 0x60000020;
	pOptionHeader->SizeOfImage += SIZEOFADDSECTION;
	pPEHeader->NumberOfSections++;
	//�ƶ��ض�λ��
	pNewBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + pNewSec->PointerToRawData);
	LPVOID Temp = pNewBasereloc;
	while (pBasereloc->VirtualAddress && pBasereloc->SizeOfBlock)
	{
		memcpy(Temp, pBasereloc, pBasereloc->SizeOfBlock);
		Temp = (PIMAGE_BASE_RELOCATION)((DWORD)Temp + pBasereloc->SizeOfBlock);
		pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pBasereloc + pBasereloc->SizeOfBlock);
	}
	//�޸�����Ŀ¼���ض�λ���ַ
	pDataDirectory[5].VirtualAddress = FoaToRva(pFileBuffer, (DWORD)pNewBasereloc - (DWORD)pFileBuffer);
	//����
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}

//����DLL���޸��ض�λ������ַ
VOID ReloadDll(DWORD RelocateImageBase)
{
	LPVOID pFileBuffer = NULL;
	int ImageOffset = 0;
	DWORD NumOfReloc = 1;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BASE_RELOCATION pBasereloc = NULL;
	DWORD size = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[5].VirtualAddress));
	BOOL isok = false;
	short* pRva = NULL;
	//������غ��ImageBaseƫ��
	ImageOffset = (signed)RelocateImageBase - (signed)pOptionHeader->ImageBase;
	//�޸�ImageBase
	pOptionHeader->ImageBase = RelocateImageBase;
	//�޸�����ƫ��ָ��ĵ�ַ��Ϣ
	while (pBasereloc->VirtualAddress && pBasereloc->SizeOfBlock)
	{
		pRva = (short*)((char*)pBasereloc + 8);
		printf("------��%d��------\n", NumOfReloc);
		for (unsigned int i = 0; i < (pBasereloc->SizeOfBlock - 8) / 2; i++)
		{
			if ((*pRva & 0xf000) == 0x3000)
			{
				//��Ҫ�޸ĵĵ�ַ
				int* AddressOffset = (int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, ((*pRva & 0xfff) + pBasereloc->VirtualAddress)));
				*AddressOffset += ImageOffset;
				printf("��%d�%x	���޸�\n", i + 1, (*pRva & 0xfff) + pBasereloc->VirtualAddress);
			}
			pRva++;
		}
		pBasereloc = (PIMAGE_BASE_RELOCATION)((DWORD)pBasereloc + pBasereloc->SizeOfBlock);
		NumOfReloc++;
	}
	//����
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}

//��ӡ�����
VOID PrintImport()
{
	LPVOID pFileBuffer;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[1].VirtualAddress));
	unsigned int* pOriginalFirstThunk = NULL;
	unsigned int* pFirstThunk = NULL;
	PIMAGE_IMPORT_BY_NAME pImportByName = NULL;
	printf("---��ӡ�����---\n");
	while (pImportDescriptor->OriginalFirstThunk != 0)
	{
		printf("ģ������%s----------------------\n", (char*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pImportDescriptor->Name)));
		printf("OriginalFirstThunk:%x\n", pImportDescriptor->OriginalFirstThunk);
		printf("TimeDateStamp:%x\n", pImportDescriptor->TimeDateStamp);
		printf("ForwarderChain:%x\n", pImportDescriptor->ForwarderChain);
		printf("Name:%x\n", pImportDescriptor->Name);
		printf("FirstThunk:%x\n", pImportDescriptor->FirstThunk);
		pOriginalFirstThunk = (unsigned int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pImportDescriptor->OriginalFirstThunk));
		printf("----------��ӡINT��OriginalFirstThunk��----------\n");
		while (*pOriginalFirstThunk != 0)
		{
			if (*pOriginalFirstThunk & 0x80000000)
			{
				printf("����������ţ�%x\n", (*pOriginalFirstThunk & 0x7fffffff));
			}
			else
			{
				pImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, *pOriginalFirstThunk));
				printf("Hint��%x\n", pImportByName->Hint);
				printf("��������%s\n", pImportByName->Name);
			}
			pOriginalFirstThunk++;
		}
		pFirstThunk = (unsigned int*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pImportDescriptor->FirstThunk));
		printf("----------��ӡIAT��FirstThunk��----------\n");
		while (*pFirstThunk != 0)
		{
			if (*pFirstThunk & 0x80000000)
			{
				printf("����������ţ�%x\n", (*pFirstThunk & 0x7fffffff));
			}
			else
			{
				pImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, *pFirstThunk));
				printf("Hint��%x\n", pImportByName->Hint);
				printf("��������%s\n", pImportByName->Name);
			}
			pFirstThunk++;
		}
		pImportDescriptor++;
	}
	free(pFileBuffer);
	return;
}

//��ӡ�󶨵����
VOID PrintBoundImport()
{
	LPVOID pFileBuffer;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BOUND_IMPORT_DESCRIPTOR pBoundImport = NULL;
	PIMAGE_BOUND_FORWARDER_REF pBoundForwarder = NULL;
	//��ȡ�ļ�
	ReadPEFile(FILEPATH_IN, 0, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pBoundImport = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[11].VirtualAddress));
	PIMAGE_BOUND_IMPORT_DESCRIPTOR pFirstDescriptor = pBoundImport;
	printf("��ӡ�󶨵����\n");
	while (pBoundImport->TimeDateStamp)
	{
		if (pBoundImport == pFirstDescriptor)
		{
			printf("TimeDateStamp:%x\n", pFirstDescriptor->TimeDateStamp);
			printf("OffsetModuleName:%s\n", (char*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pFirstDescriptor->OffsetModuleName)));
			printf("NumberOfModuleForwarderRefs:%x\n", pFirstDescriptor->NumberOfModuleForwarderRefs);
		}
		else
		{
			printf("TimeDateStamp:%x\n", pBoundImport->TimeDateStamp);
			printf("OffsetModuleName:%s\n", (char*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, (pBoundImport->OffsetModuleName + pFirstDescriptor->OffsetModuleName))));
			printf("NumberOfModuleForwarderRefs:%x\n", pBoundImport->NumberOfModuleForwarderRefs);
		}
		pBoundForwarder = (PIMAGE_BOUND_FORWARDER_REF)(pBoundImport + 1);
		printf("----------\n");
		for (size_t i = 0; i < pBoundImport->NumberOfModuleForwarderRefs; i++)
		{
			printf("TimeDateStamp:%x\n", pBoundForwarder->TimeDateStamp);
			printf("OffsetModuleName:%s\n", (char*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, (pBoundForwarder->OffsetModuleName + pFirstDescriptor->OffsetModuleName))));
			printf("NumberOfModuleForwarderRefs:%x\n", pBoundForwarder->Reserved);
			pBoundForwarder++;
		}
		pBoundImport = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)pBoundForwarder;
	}
	free(pFileBuffer);
	return;
}

//�����ע��
VOID ImportInject()
{
	LPVOID pFileBuffer;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pNewSec = NULL;//�½ڱ�ṹ
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pNewImportDescriptor = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pInjectImport = NULL;
	PIMAGE_IMPORT_BY_NAME pImportByName = NULL;
	size_t i = 0;
	DWORD size = 0;
	BOOL isok = false;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, SIZEOFADDSECTION, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[1].VirtualAddress));
	DWORD NumOfDll = 0;
	//���������
	while (pImportDescriptor[NumOfDll].OriginalFirstThunk)
	{
		NumOfDll++;
	}
	DWORD NeedSize = (sizeof(*pImportDescriptor) * (NumOfDll + 1)) + 20 + 16 + 44;
	//�жϽڱ���Ƿ��ж���ռ���ӽڱ�
	for (; i < 80; i++)
	{
		if (*((char*)(pSectionHeader + pPEHeader->NumberOfSections) + i) != 0)
		{
			printf("�ڱ���޶���ռ䣬��������PEͷ������пռ�\n");
			//����DOSͷ��NTͷ֮�����������
			memmove((char*)pFileBuffer + 0x40, (char*)pFileBuffer + pDosHeader->e_lfanew, (DWORD)(pSectionHeader + pPEHeader->NumberOfSections) - (DWORD)pFileBuffer - pDosHeader->e_lfanew);
			//����pDosHeader->e_lfanew
			int x = pDosHeader->e_lfanew;
			pDosHeader->e_lfanew = 0x40;
			//���¸�ͷָ�븳ֵ
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			//�����Ǻ������������0
			memset(pSectionHeader + pPEHeader->NumberOfSections, 0, x - 0x40);
			break;
		}
	}
	//��ӽڱ�
	if (i == 80)
	{
		printf("�ڱ���ж���ռ䣬ֱ������½ڱ�\n");
	}
	//�����ڱ�ṹ
	pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	//��д�����ڱ������
	unsigned char arr[8] = ".Inject";
	memcpy(pNewSec->Name, arr, 8);
	pNewSec->Misc.VirtualSize = SIZEOFADDSECTION;
	if (pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize > pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData)
	{
		pNewSec->VirtualAddress = Align((pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].Misc.VirtualSize), pOptionHeader->SectionAlignment);
	}
	else {
		pNewSec->VirtualAddress = Align(pSectionHeader[pPEHeader->NumberOfSections - 1].VirtualAddress + pSectionHeader[pPEHeader->NumberOfSections - 1].SizeOfRawData, pOptionHeader->SectionAlignment);
	}
	pNewSec->SizeOfRawData = SIZEOFADDSECTION;
	pNewSec->PointerToRawData = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (pSectionHeader + pPEHeader->NumberOfSections - 1)->SizeOfRawData;
	pNewSec->PointerToRelocations = 0;
	pNewSec->PointerToLinenumbers = 0;
	pNewSec->NumberOfRelocations = 0;
	pNewSec->NumberOfLinenumbers = 0;
	pNewSec->Characteristics = 0xC0000040;
	pOptionHeader->SizeOfImage += SIZEOFADDSECTION;
	pPEHeader->NumberOfSections++;
	//DWORD i = 0;//��¼Ҫ��ӽڵ�λ��
	////�жϸ��ں��Ƿ��пռ����
	//for (; i < pPEHeader->NumberOfSections; i++)
	//{
	//	DWORD j = 0;
	//	for (; j < NeedSize; j++)
	//	{
	//		if (pSectionHeader[i].Misc.VirtualSize > pSectionHeader[i].SizeOfRawData || *((char*)pFileBuffer + pSectionHeader[i].PointerToRawData + pSectionHeader[i].Misc.VirtualSize + 16))
	//		{
	//			printf("��%d����û�пռ��ƶ������\n", i+1);
	//			break;
	//		}
	//	}
	//	if (j == NeedSize)
	//	{
	//		printf("��%d�����пռ䣬�ڴ˽��ƶ������\n", i+1);
	//		break;
	//	}
	//}
	//if (i == pPEHeader->NumberOfSections)
	//{
	//	printf("û�п���ڣ����������ƶ������\n");
	//	free(pFileBuffer);
	//	return;
	//}
	//���µ�����ַ��ֵ
	pNewImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + pNewSec->PointerToRawData);
	//��ʼ�ƶ������
	memcpy((char*)pNewImportDescriptor, (char*)pImportDescriptor, (sizeof(*pImportDescriptor) * NumOfDll));//�ƶ�ԭ���ĵ�������нں�
	//��¼ע��ĵ�����Լ�INT���IAT������ֱ�
	pInjectImport = pNewImportDescriptor + NumOfDll;
	PDWORD pINT = (PDWORD)(pInjectImport + 2);
	PDWORD pIAT = (PDWORD)(pINT + 2);
	pImportByName = (PIMAGE_IMPORT_BY_NAME)(pIAT + 2);
	char FounctionName[] = "ExportFunction";
	char DllName[] = "InjectDll.dll";
	//���������������ֱ�
	strcpy((char*)pImportByName + 2, FounctionName);
	//����dll��
	strcpy((char*)pImportByName + 3 + strlen(FounctionName), DllName);
	//����������ֵ
	pInjectImport->OriginalFirstThunk = FoaToRva(pFileBuffer, (DWORD)pINT - (DWORD)pFileBuffer);
	pInjectImport->Name = FoaToRva(pFileBuffer, (DWORD)pImportByName + 3 + strlen(FounctionName) - (DWORD)pFileBuffer);
	pInjectImport->FirstThunk = FoaToRva(pFileBuffer, (DWORD)pIAT - (DWORD)pFileBuffer);
	*pINT = FoaToRva(pFileBuffer, (DWORD)pImportByName - (DWORD)pFileBuffer);
	*pIAT = FoaToRva(pFileBuffer, (DWORD)pImportByName - (DWORD)pFileBuffer);
	//�޸�����Ŀ¼������ֵ
	pDataDirectory[1].VirtualAddress = FoaToRva(pFileBuffer, (DWORD)pNewImportDescriptor - (DWORD)pFileBuffer);
	pDataDirectory[1].Size += 20;
	//��ע����exe���浽Ӳ����
	isok = MeneryToFile(pFileBuffer, size, FILEPATH_OUT);
	if (isok)
	{
		printf("���̳ɹ���\n");
	}
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}

//������Դ��
void SreachResource()
{
	LPVOID pFileBuffer;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_RESOURCE_DIRECTORY pResourceType = NULL;//��Դ���ͱ�
	PIMAGE_RESOURCE_DIRECTORY pResourceNumber = NULL;//��Դ�ı�ű�
	PIMAGE_RESOURCE_DIRECTORY pResourceDirCodePage = NULL;//����ҳ��
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntry = NULL;//��ԴĿ¼��
	PIMAGE_RESOURCE_DATA_ENTRY pResourceData = NULL;//��Դ�ڵ�ָ���
	PIMAGE_RESOURCE_DIR_STRING_U pNamew = NULL;
	DWORD size = 0;
	//��ȡ�ļ�
	size = ReadPEFile(FILEPATH_IN, SIZEOFADDSECTION, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("��ȡ�ļ�ʧ��\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)(pOptionHeader->DataDirectory);
	pResourceType = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pDataDirectory[2].VirtualAddress));
	printf("��Դ��������Ϊ��%d\n", pResourceType->NumberOfNamedEntries + pResourceType->NumberOfIdEntries);
	//������Դ���ͱ�
	for (int i = 0; i < (pResourceType->NumberOfNamedEntries + pResourceType->NumberOfIdEntries); i++)
	{
		pResourceEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pResourceType + sizeof(*pResourceType) + (i * 8));
		if (pResourceEntry->NameIsString)
		{
			pNamew = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pResourceEntry->NameOffset));
			wprintf(L"��Դ���ͣ�%s\n", pNamew->NameString);
		}
		else
		{
			printf("��Դ���ͣ�%d\n", pResourceEntry->NameOffset);
		}
		pResourceNumber = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResourceType + pResourceEntry->OffsetToDirectory);
		printf("������Դ��ĿΪ��%d\n", pResourceNumber->NumberOfNamedEntries + pResourceNumber->NumberOfIdEntries);
		//������Դ��ű�
		for (int j = 0; j < (pResourceNumber->NumberOfIdEntries + pResourceNumber->NumberOfNamedEntries); j++)
		{
			pResourceEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pResourceNumber + sizeof(*pResourceNumber) + (j * 8));
			if (pResourceEntry->NameIsString)
			{
				pNamew = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pResourceEntry->NameOffset));
				wprintf(L"��Դ��ţ�%s\n", pNamew->NameString);
			}
			else
			{
				printf("��Դ��ţ�%d\n", pResourceEntry->NameOffset);
			}
			pResourceDirCodePage = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResourceType + pResourceEntry->OffsetToDirectory);
			//��������ҳ��
			for (int k = 0; k < (pResourceDirCodePage->NumberOfIdEntries + pResourceDirCodePage->NumberOfNamedEntries); k++)
			{
				pResourceEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pResourceDirCodePage + sizeof(*pResourceDirCodePage) + (k * 8));
				if (pResourceEntry->NameIsString)
				{
					pNamew = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer, pResourceEntry->NameOffset));
					wprintf(L"����ҳ��%s\n", pNamew->NameString);
				}
				else
				{
					printf("����ҳ��%d\n", pResourceEntry->NameOffset);
				}
				//��ӡ��������Դ��ַ
				pResourceData = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pResourceType + pResourceEntry->OffsetToDirectory);
				printf("��Դ��ַ��%x", pResourceData->OffsetToData);
				printf("��Դ��С��%x\n", pResourceData->Size);
			}
			printf("-----------------------------------------------------\n");
		}
		printf("-----------------------------------------------------\n");
	}
	free(pFileBuffer);
	pFileBuffer = NULL;
	return;
}

#pragma once
#include <stdio.h>
#include <Windows.h>

#pragma comment(lib,"ntdll.lib")

EXTERN_C NTSTATUS NTAPI NtTerminateProcess(HANDLE, NTSTATUS);
EXTERN_C NTSTATUS NTAPI NtReadVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);
EXTERN_C NTSTATUS NTAPI NtWriteVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);
EXTERN_C NTSTATUS NTAPI NtGetContextThread(HANDLE, PCONTEXT);
EXTERN_C NTSTATUS NTAPI NtSetContextThread(HANDLE, PCONTEXT);
EXTERN_C NTSTATUS NTAPI NtUnmapViewOfSection(HANDLE, PVOID);
EXTERN_C NTSTATUS NTAPI NtResumeThread(HANDLE, PULONG);

DWORD ReadPEFile(IN LPSTR lpszFile, IN DWORD offset, OUT LPVOID* pFileBuffer);//��ȡ�ļ�
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer);//����FileBuffer��ImageBuffer
void TcharToChar(const TCHAR* tchar, char* _char);//TCHAR����תΪCHAR����
BOOL GetSrcInfo(LPVOID pFileBuffer, DWORD* SizeOfLastSec, DWORD* SrcImageBase, DWORD* SrcOEP, LPVOID* pSrcFileBuffer);//��ȡԴ�ļ�����Ϣ
PROCESS_INFORMATION CreateProcessSuspend(LPSTR processName);//�Թ���ķ�ʽ��������
DWORD UnmapShell(HANDLE hProcess, DWORD shellImageBase);//ж�ؽ�������
CONTEXT GetThreadContext(HANDLE hThread);//��ȡ�߳�Context��Ϣ
LPVOID VirtualAllocate(HANDLE hProcess, PVOID pAddress, DWORD size_t);//�����ڴ浽ָ��λ��
DWORD GetProcessImageBase(PROCESS_INFORMATION procInfo);//��ȡ���̻�ַ
CONTEXT GetThreadContext(HANDLE hThread);//��ȡ�߳�Context��Ϣ
LPVOID Xor(IN LPVOID pBuffer, DWORD size);//������Դ�ļ�
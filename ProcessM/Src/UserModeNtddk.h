/* --------------------------------------------------------------------------------------------*/
//	文件名		:	UserModeNtddk.h
//	目的		:	导出的一些结构体或者定义。
//	创建时间	:	2014-04-10 18:25
//	创建者		:	IDeath
//
//	历史记录	:	
/* --------------------------------------------------------------------------------------------*/
#pragma once
#include <Wtypes.h>
#include <Winternl.h>

namespace UserModeKernel
{
#ifndef STATUS_INFO_LENGTH_MISMATCH
#	define STATUS_INFO_LENGTH_MISMATCH	((NTSTATUS)0xC0000004L)
#endif

#ifndef SystemModuleInformation
#	define SystemModuleInformation 11
#endif
	typedef NTSTATUS(WINAPI *pFunc_ZwQueryInformationProcess)(
		_In_       HANDLE ProcessHandle,
		_In_       PROCESSINFOCLASS ProcessInformationClass,
		_Out_      PVOID ProcessInformation,
		_In_       ULONG ProcessInformationLength,
		_Out_opt_  PULONG ReturnLength
		);

	typedef NTSTATUS (WINAPI *pFunc_ZwQuerySystemInformation)(
		_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
		_Inout_    PVOID SystemInformation,
		_In_       ULONG SystemInformationLength,
		_Out_opt_  PULONG ReturnLength
		);

	typedef NTSTATUS (*pFunc_NtCreateFile)(
		_Out_     PHANDLE FileHandle,
		_In_      ACCESS_MASK DesiredAccess,
		_In_      POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_     PIO_STATUS_BLOCK IoStatusBlock,
		_In_opt_  PLARGE_INTEGER AllocationSize,
		_In_      ULONG FileAttributes,
		_In_      ULONG ShareAccess,
		_In_      ULONG CreateDisposition,
		_In_      ULONG CreateOptions,
		_In_      PVOID EaBuffer,
		_In_      ULONG EaLength
		);
	
	typedef struct _SYSTEM_MODULE_INFORMATION {
		ULONG Reserved[2];
		PVOID Base;
		ULONG Size;
		ULONG Flags;
		USHORT Index;
		USHORT Unknown;
		USHORT LoadCount;
		USHORT ModuleNameOffset;
		CHAR ImageName[256];
	} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

	typedef struct {
		ULONG  dwNumberOfModules;
		SYSTEM_MODULE_INFORMATION  smi;
	} MODULES, *PMODULES;
}
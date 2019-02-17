#include "stdafx.h"
#include "ProcessManager.h"
#include "StrConversion.h"
#include "ErrorLog.h"

#include <TlHelp32.h>
#include <Psapi.h>
#include <intsafe.h>

#include <atlutil.h>
#include <atlsecurity.h>

#include <time.h>
#include <Winternl.h>

using namespace UserModeKernel;
#pragma comment(lib, "Version.lib")

BOOL CProcessManagerIMPL::DisplayErrorInformation(TCHAR *pszAPI, DWORD dwError)
{
	LPVOID lpvMessageBuffer = NULL;
	DWORD dwResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
								   FORMAT_MESSAGE_FROM_SYSTEM |
								   FORMAT_MESSAGE_IGNORE_INSERTS,
								   NULL,
								   dwError,
								   MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
								   (LPTSTR)&lpvMessageBuffer,
								   0,
								   NULL);
	if (dwResult != 0 && lpvMessageBuffer != NULL)
	{	
		DebugPrint(_T("Error API = [%s] Code = [%d] Message = %s"), pszAPI, dwError, lpvMessageBuffer);
		LocalFree(lpvMessageBuffer);
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::ICreateProcess(PROCESS_INFORMATION& ProcessInformation,
									LPSTR lpCommandLine,
									BOOL bInheritHandles,
									DWORD dwWaitTime /* = INFINITE */)
{
	STARTUPINFOA StartInformation = { 0 };
	BOOL bResult = ::CreateProcessA(NULL,
		lpCommandLine,
		NULL,
		NULL,
		bInheritHandles,
		0,
		NULL,
		NULL,
		&StartInformation,
		&ProcessInformation);
	if (!bResult)
	{
		DebugPrint(_T("CreateProcess Failed [%d]"), GetLastError());
	}
	else
	{
		DWORD dwResult = WaitForInputIdle(ProcessInformation.hProcess, 5000);
		if (dwResult == WAIT_TIMEOUT)
		{
			DebugPrint(_T("Wait For Initialize Finished Time Out!!"));
		}
		else
		{
			WaitForSingleObject(ProcessInformation.hProcess, dwWaitTime);
		}
		CloseHandle(ProcessInformation.hThread);
		CloseHandle(ProcessInformation.hProcess);
	}
	return bResult;
}

BOOL CProcessManagerIMPL::CreateProcessWithLoginUser(PROCESS_INFORMATION& ProcessInformation, 
												 std::string szUserName,
												 std::string szDomain,
												 std::string szPassWord,
												 std::string szApplicationPath,
												 LPSTR lpCommandLine)
{
	QUOTA_LIMITS QuotaLimit = { 0 };
	DWORD dwProFileLenght	= 0;
	HANDLE hUserToken		= NULL;
	PSID pUserSecurityID	= NULL;
	PVOID pUserProFileBuffer= NULL;
	BOOL bResult = FALSE;
	do 
	{
		if (!LogonUserExA(szUserName.c_str(),
						  szDomain.c_str(),
						  szPassWord.c_str(),
						  LOGON32_LOGON_INTERACTIVE,
						  LOGON32_PROVIDER_DEFAULT,
						  &hUserToken,
						  &pUserSecurityID,
						  &pUserProFileBuffer,
						  &dwProFileLenght,
						  &QuotaLimit) && NULL != hUserToken)
		{
			break;
		}

		STARTUPINFOA StartInformation = { 0 };
		if (CreateProcessAsUserA(hUserToken,
								 szApplicationPath.c_str(),
								 lpCommandLine,
								 NULL,
								 NULL,
								 FALSE,
								 0,
								 NULL,
								 NULL,
								 &StartInformation,
								 &ProcessInformation))
		{
			bResult = TRUE;
		}
		else
		{
			if (GetLastError() == ERROR_PRIVILEGE_NOT_HELD)
			{
				STARTUPINFOW StartInformation = { 0 };
				BOOL bResutl = CreateProcessWithLogonW(StrConversion::AnsiToWchar(szUserName).c_str(),
													   StrConversion::AnsiToWchar(szDomain).c_str(),
													   StrConversion::AnsiToWchar(szPassWord).c_str(),
													   LOGON_WITH_PROFILE,
													   StrConversion::AnsiToWchar(szApplicationPath).c_str(),
													   (LPWSTR)StrConversion::AnsiToWchar(std::string(lpCommandLine)).c_str(),
													   0,
													   NULL,
													   NULL,
													   &StartInformation,
													   &ProcessInformation);
				if (!bResutl)
				{
					DebugPrint(_T("CreateProcessWithLogonW Failed [%d]"), GetLastError());
					break;
				}
				bResult = TRUE;
			}
		}
	} while (false);

	if (bResult)
	{
		WaitForInputIdle(ProcessInformation.hProcess, 5000);
		CloseHandle(ProcessInformation.hThread);
		CloseHandle(ProcessInformation.hProcess);
	}
	else
	{
		DebugPrint(_T("Logon User Failed [%d]"), GetLastError());
	}
	return bResult;
}

BOOL CProcessManagerIMPL::DisplayCallerAccessTokenInFormation()
{
	HANDLE hToken = NULL;
	BOOL bResult = OpenThreadToken(GetCurrentThread(),
								   TOKEN_QUERY|TOKEN_QUERY_SOURCE,
								   TRUE,
								   &hToken);
	if (bResult == FALSE && GetLastError() == ERROR_NO_TOKEN)
	{
		bResult = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_QUERY_SOURCE, &hToken);
	}

	if (!bResult || hToken == NULL)
	{
		DebugPrint(_T("Error In Open Token [%d]"), GetLastError());
		return FALSE;
	}
	else
	{
		bResult = DisplayTokenInfomation(hToken);
		CloseHandle(hToken);
	}
	return bResult;
}

std::shared_ptr<BYTE> CProcessManagerIMPL::RetrieveTokenInFormationClass(HANDLE hToken, TOKEN_INFORMATION_CLASS nTokenClass, LPDWORD lpdwSize)
{
	std::shared_ptr<BYTE> pData;
	try
	{
		ZeroMemory(lpdwSize, sizeof(DWORD));
		GetTokenInformation(hToken,
							nTokenClass,
							NULL,
							*lpdwSize,
							lpdwSize);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			DebugPrint(_T("GetTokenInformation failed with %d\n"), GetLastError());
			return pData;
		}

		pData = std::shared_ptr<BYTE>(new BYTE[*lpdwSize]);
		if (pData != NULL)
		{
			if (!GetTokenInformation(hToken,
									 nTokenClass,
									 pData.get(),
									 *lpdwSize, 
									 lpdwSize))
			{
				*lpdwSize = 0;
				DebugPrint(_T("GetTokenInformation failed with %d\n"), GetLastError());
				return pData;
			}
		}
	}
	catch (...)
	{
		DebugPrint(_T("Exception Occur In  RetrieveTokenInFormationClass [%d]\n"), GetLastError());
	}
	return pData;
}

BOOL CProcessManagerIMPL::RetrievtTokenUserData(HANDLE hToken, std::string& strUserName)
{
	DWORD dwSize = 0;
	std::shared_ptr<BYTE> pData = RetrieveTokenInFormationClass(hToken, TokenUser, &dwSize);
	if (pData != NULL && dwSize != 0)
	{
		TOKEN_USER *pUser = static_cast<TOKEN_USER*>((TOKEN_USER*)pData.get());
		if (pUser != NULL)
		{
			if (IsValidSid(pUser->User.Sid))
			{
				LPSTR pUserName = NULL;
				if (ConvertSidToStringSidA(pUser->User.Sid, &pUserName) && NULL != pUserName)
				{
					strUserName = pUserName;
					LocalFree(pUserName);
					return TRUE;
				}
				else
				{
					DebugPrint(_T("ConvertSidToStringSid Failed [%d]"), GetLastError());
				}
			}
		}
	}
	return FALSE;
}

BOOL CProcessManagerIMPL::DisplayTokenInfomation(HANDLE hToken)
{
	std::string strUserName;
	RetrievtTokenUserData(hToken, strUserName);
	return TRUE;
}

BOOL CProcessManagerIMPL::SetCurrentProcessDebugPrivileges()
{
	HANDLE hToken = NULL;
	BOOL bResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (bResult && hToken != NULL)
	{
		bResult = AdjustProcessPrivileges(hToken, SE_DEBUG_NAME, TRUE);
		CloseHandle(hToken);
	}
	return bResult;
}

BOOL CProcessManagerIMPL::AdjustProcessPrivileges(HANDLE hToken, LPCTSTR szPrivileges, BOOL bEnable)
{
	TOKEN_PRIVILEGES PrivilegesData = { 0 };
	PrivilegesData.PrivilegeCount = 1;
	BOOL bResult = LookupPrivilegeValue(NULL, szPrivileges, &PrivilegesData.Privileges[0].Luid);
	if (!bResult)
	{
		DebugPrint(_T("LookupPrivilegeValue [%s]"), GetLastError());
		return FALSE;
	}
	
	DWORD dwLength = 0;
	bEnable ? PrivilegesData.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED
		    : PrivilegesData.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;

	bResult = AdjustTokenPrivileges(hToken,
		FALSE,
		&PrivilegesData,
		sizeof(PrivilegesData),
		NULL,
		&dwLength);

	if (!bResult || GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		DebugPrint(_T("Adjust Token Failed 此权限不是当前用户拥有 [%d]"), GetLastError());
		return FALSE;
	}
	return bResult;
}

DWORD CProcessManagerIMPL::GetCurrentProcessID()
{
	return GetCurrentProcessId();
}

HANDLE CProcessManagerIMPL::GetCurrentProcessHandle(BOOL bRealHandle /* = FALSE */)
{
	if (bRealHandle)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessID());
	}
	else
	{
		return GetCurrentProcess();
	}
}

BOOL CProcessManagerIMPL::CrossProcessDuplicateHandle(HANDLE hToDuplicated, DWORD dwDestProcessID, HANDLE& hDuplicate)
{
	HANDLE hDestProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwDestProcessID);
	if (NULL == hDestProcess)
	{
		DebugPrint(_T("Open Process With PROCESS_DUP_HANDLE Failed [%d]\n"), GetLastError());
		return FALSE;
	}
	
	BOOL bResult = DuplicateHandle(GetCurrentProcess(),
								   hToDuplicated,
								   hDestProcess,
								   &hDuplicate,
								   0,
								   FALSE,
								   DUPLICATE_SAME_ACCESS);
	if (!bResult)
	{
		DebugPrint(_T("Error In Duplicate Handle [%d]\n"), GetLastError());
	}
	CloseHandle(hDestProcess);
	return bResult;
}

BOOL CProcessManagerIMPL::CloseDuplicateHandle(HANDLE hDestProcess, HANDLE hDuplicate)
{
	BOOL bResult = DuplicateHandle(hDestProcess,
								   hDuplicate,
								   NULL,
								   NULL,
								   0,
								   FALSE,
								   DUPLICATE_CLOSE_SOURCE);
	if (!bResult)
	{
		DebugPrint(_T("Error In Closr Duplicate Handle [%d]\n"), GetLastError());
	}
	return bResult;
}

BOOL CProcessManagerIMPL::EnumProcessData()
{
	DWORD arrProcessID[1000] = { 0 };
	DWORD dwNeedSize = 0, dwSizeStoreID = sizeof(arrProcessID);
	if (EnumProcesses(arrProcessID, dwSizeStoreID, &dwNeedSize))
	{
		if (dwNeedSize < dwSizeStoreID)
		{
			UINT nProcessCount = dwNeedSize / sizeof(DWORD);
			for (UINT nIndex = 0; nIndex < nProcessCount; ++nIndex)
			{
				EnumProcessModulesData(arrProcessID[nIndex]);
				ShowProcessMemoryStatus(arrProcessID[nIndex]);
			}
		}
		else
		{
			DebugPrint(_T("Enum Array Is To Small\n"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::EnumProcessModulesData(DWORD dwProcessID)
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
	if (NULL == hProcess)
	{
		return FALSE;
	}
	else
	{
		HMODULE hModuleList[1024] = { 0 };
		DWORD dwNeedSize = 0;
		if (EnumProcessModulesEx(hProcess, hModuleList, sizeof(hModuleList), &dwNeedSize, LIST_MODULES_ALL))
		{
			if (dwNeedSize < sizeof(hModuleList))
			{
				UINT nMoudleCount = dwNeedSize / sizeof(HMODULE);
				for (UINT nIndex = 0; nIndex < nMoudleCount; ++nIndex)
				{
					TCHAR szMoudleBuffer[512] = { 0 };
					GetModuleBaseName(hProcess, hModuleList[nIndex], szMoudleBuffer, sizeof(szMoudleBuffer));

					TCHAR szFileName[512] = { 0 };
					GetModuleFileNameEx(hProcess, hModuleList[nIndex], szFileName, sizeof(szFileName));

					MODULEINFO ModuleInfo;
					GetModuleInformation(hProcess, hModuleList[nIndex], &ModuleInfo, sizeof(MODULEINFO));
					DebugPrint(_T("SizeOfImage = [%d] lpBaseOfDll = [%p] EntryPoint = [%p] ModuleBaseName = [%s] ModuleFileName = [%s]\n")
						, ModuleInfo.SizeOfImage, ModuleInfo.lpBaseOfDll, ModuleInfo.EntryPoint, szMoudleBuffer, szFileName);
					
					DWORD dwFullImageName = 0;
					TCHAR szFullImageName[512] = { 0 };
					QueryFullProcessImageName(hProcess, PROCESS_NAME_NATIVE, szFullImageName, &dwFullImageName);
				}
			}
			else
			{
				CloseHandle(hProcess);
				DebugPrint(_T("No Enough Space To Store Module List\n"));
				return FALSE;
			}
		}
		CloseHandle(hProcess);
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::ShowProcessMemoryStatus(DWORD dwProcessID)
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
	if (NULL == hProcess)
	{
		return FALSE;
	}
	else
	{
		PROCESS_MEMORY_COUNTERS_EX PmCountEx;
		if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&PmCountEx, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
		{
			DebugPrint(_T("Page Fault Count(页面错误) [%d] Page File (提交大小) [%d] PrivateUsage（提交大小 < WIN7系统使用） = [%d] PeakPagefileUsage(峰值提交大小) = [%d]\n\
						   PeakWorkingSetSize（峰值工作设置） = [%d] QuotaNonPagedPoolUsage（非分页池） = [%d] QuotaPagedPoolUsage（分页池） = [%d] \n\
						   QuotaPeakNonPagedPoolUsage（峰值非分页池） = [%d] QuotaPeakPagedPoolUsage（峰值分页池） = [%d] WorkingSetSize(工作设置) = [%d]\n")
				, PmCountEx.PageFaultCount
				, PmCountEx.PagefileUsage / 1024
				, PmCountEx.PrivateUsage / 1024
				, PmCountEx.PeakPagefileUsage / 1024
				, PmCountEx.PeakWorkingSetSize / 1024
				, PmCountEx.QuotaNonPagedPoolUsage / 1024
				, PmCountEx.QuotaPagedPoolUsage / 1024
				, PmCountEx.QuotaPeakNonPagedPoolUsage / 1024
				, PmCountEx.QuotaPeakPagedPoolUsage / 1024
				, PmCountEx.WorkingSetSize / 1024
				);
		}
		CloseHandle(hProcess);
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::ShowProcessPerformanceStatus()
{
	PERFORMACE_INFORMATION PerformanceData;
	if (GetPerformanceInfo(&PerformanceData, sizeof(PERFORMACE_INFORMATION)))
	{
		DebugPrint(_T("CommitLimit = [%d] CommitPeak = [%d] HandleCount = [%d] KernelNonpaged = [%d] \n\
					  KernelPaged = [%d] KernelTotal = [%d] PageSize = [%d] PhysicalAvailable = [%d] \n\
					  PhysicalTotal = [%d] ProcessCount = [%d] ThreadCount = [%d] SystemCache = [%d]")
			, PerformanceData.CommitLimit
			, PerformanceData.CommitPeak
			, PerformanceData.CommitTotal
			, PerformanceData.HandleCount
			, PerformanceData.KernelNonpaged
			, PerformanceData.KernelPaged
			, PerformanceData.KernelTotal
			, PerformanceData.PageSize
			, PerformanceData.PhysicalAvailable
			, PerformanceData.PhysicalTotal
			, PerformanceData.ProcessCount
			, PerformanceData.ThreadCount
			, PerformanceData.SystemCache
			);
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::ShowProcessHeap()
{
	DWORD dwNumberHeaps = GetProcessHeaps(0, NULL);
	
	SIZE_T sizeByteToAllocate = 0;
	if (SIZETMult(dwNumberHeaps, sizeof(PHANDLE), &sizeByteToAllocate) != S_OK)
	{
		DebugPrint(_T("A ha Error [%d]"), GetLastError());
		return FALSE;
	}
	
	HANDLE hProcessHeap = GetProcessHeap();
	if (hProcessHeap == NULL)
	{
		DebugPrint(_T("Failed To Retrive The Default Process Heap [%d]"), GetLastError());
		return FALSE;
	}

	PHANDLE ProcessHeaps = (PHANDLE)HeapAlloc(hProcessHeap, HEAP_ZERO_MEMORY, sizeByteToAllocate);
	if (ProcessHeaps != NULL)
	{
		if (GetProcessHeaps(dwNumberHeaps, ProcessHeaps) == dwNumberHeaps)
		{
			for (UINT nIndex = 0; nIndex < dwNumberHeaps; ++nIndex)
			{
				DebugPrint(_T("Headp nIndex = [%d] Address = [%p]\n"), nIndex, ProcessHeaps[nIndex]);
			}
		}
		else
		{
			DebugPrint(_T("Heaps Buffer To Small"));
		}
		HeapFree(hProcessHeap, 0, ProcessHeaps);
	}
	return TRUE;
}

BOOL CProcessManagerIMPL::IsWow64Process(UINT nPID)
{
	BOOL bResult = FALSE;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, nPID);
	if (hProcess != NULL)
	{
		//此API的路径不是直接的文件路径。
		TCHAR szProcssImagePath[512] = { 0 };
		if (GetProcessImageFileName(hProcess, szProcssImagePath, sizeof(szProcssImagePath)) != 0)
		{
			DWORD dwHandle = 0;
			INT nResult = GetFileVersionInfoSize(szProcssImagePath, &dwHandle);
			switch (nResult)
			{
			case 64:
				bResult = TRUE;
				DebugPrint(_T("PID [%d] Path [%s]= 64 -- Process"), nPID, szProcssImagePath);
				break;
			case 32:
				DebugPrint(_T("PID [%d] Path [%s]= 32 -- Process"), nPID, szProcssImagePath);
				break;
			default:
				DebugPrint(_T("PID [%d] Path [%s] Error = [%d]"), nPID, szProcssImagePath, GetLastError());
				break;
			}
		}

		if (!bResult)
		{
			typedef BOOL (WINAPI *pFunc_IsWow64Process)(
				_In_   HANDLE hProcess,
				_Out_  PBOOL Wow64Process
				);

			pFunc_IsWow64Process pFunIsWow64Process = (pFunc_IsWow64Process)GetProcAddress(
				GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

			if (pFunIsWow64Process != NULL)
			{
				if (!pFunIsWow64Process(hProcess, &bResult))
				{
					DebugPrint(_T("Error = [%d]"), GetLastError());
				}
			}
		}
		CloseHandle(hProcess);
	}
	return bResult;
}

BOOL CProcessManagerIMPL::CheckFunctionEntryCode(PBYTE pFunctionAddress)
{
	if (*pFunctionAddress == 0xE9)
	{
		return FALSE;
	}
	return TRUE;
}

std::vector<DWORD> CProcessManagerIMPL::GetProcessPidByName(LPCTSTR lpszProcName)
{
	PROCESSENTRY32 ProcessEntry32 = { 0 };
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
	std::vector<DWORD> vPID;
	
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		Process32First(hSnapshot, &ProcessEntry32);
		do
		{
			if (!lstrcmpi(lpszProcName, ProcessEntry32.szExeFile))
			{
				vPID.push_back(ProcessEntry32.th32ProcessID);
			}
			ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
		} while (Process32Next(hSnapshot, &ProcessEntry32));
		CloseHandle(hSnapshot);
	}
	return vPID;
}

std::string CProcessManagerIMPL::GetNameByPid(UINT nPID)
{
	PROCESSENTRY32 ProcessEntry32 = { 0 };
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
	std::string strExeFile;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		Process32First(hSnapshot, &ProcessEntry32);
		do
		{
			if (ProcessEntry32.th32ProcessID == nPID)
			{
				strExeFile = std::string(ProcessEntry32.szExeFile);
				break;
			}
		} while (Process32Next(hSnapshot, &ProcessEntry32));
		CloseHandle(hSnapshot);
	}
	return strExeFile;
}

bool CProcessManagerIMPL::IsProcessRunning(LPCTSTR lpszProcName, UINT nPID)
{
	PROCESSENTRY32 ProcessEntry32 = { 0 };
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
	bool bExist = false;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		Process32First(hSnapshot, &ProcessEntry32);
		do
		{
			if (!lstrcmpi(lpszProcName, ProcessEntry32.szExeFile) 
				&& ProcessEntry32.th32ProcessID == nPID)
			{
				bExist = true;
				break;
			}
		} while (Process32Next(hSnapshot, &ProcessEntry32));
		CloseHandle(hSnapshot);
	}
	return bExist;
}

void CProcessManagerIMPL::KillProcessByName(LPCTSTR lpszProcName)
{
	std::vector<DWORD> vPID = GetProcessPidByName(lpszProcName);
	for (DWORD dwPID : vPID)
	{
		KillProcessByPID(dwPID);
	}
}

BOOL CProcessManagerIMPL::KillProcessByPID(UINT nPID)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, nPID);
	if (NULL != hProcess)
	{
		return KillProcessByHande(hProcess);
	}
	return FALSE;
}

BOOL CProcessManagerIMPL::KillProcessByHande(HANDLE hProcess)
{
	BOOL bResult = FALSE;
	try
	{
		bResult = TerminateProcess(hProcess, 0);
		if (bResult)
		{
			CloseHandle(hProcess);
		}
	}
	catch (...){ }
	return bResult;
}
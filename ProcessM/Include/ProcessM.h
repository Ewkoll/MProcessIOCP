/*****************************************************************************
*  @file     ProcessM.h
*  @brief    导出进程信息获取接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 14:30:56
*****************************************************************************/
#ifndef ProcessM_h__
#define ProcessM_h__

#pragma once
#include "Common\Global.h"

class CProcessManagerIMPL;

class DLL_EXPORT CProcessM
{
public:

	CProcessM();

	~CProcessM();

	BOOL ICreateProcess(PROCESS_INFORMATION& ProcessInformation, LPSTR lpCommandLine, BOOL bInheritHandles, DWORD dwWaitTime = INFINITE);
	
	BOOL CreateProcessWithLoginUser(PROCESS_INFORMATION& ProcessInformation, std::string szUserName, std::string szDomain, std::string szPassWord, std::string szApplicationPath, LPSTR lpCommandLine);
	
	BOOL SetCurrentProcessDebugPrivileges();

	DWORD GetCurrentProcessID();

	HANDLE GetCurrentProcessHandle(BOOL bRealHandle = FALSE);

	BOOL CrossProcessDuplicateHandle(HANDLE hToDuplicated, DWORD dwDestProcessID, HANDLE& hDuplicate);

	BOOL CloseDuplicateHandle(HANDLE hDestProcess, HANDLE hDuplicate);

	BOOL IsWow64Process(UINT nPID);
	
	std::vector<DWORD> GetProcessPidByName(std::string strProcName);

	std::string GetNameByPid(UINT nPID);

	bool IsProcessRunning(std::string strProcName, UINT nPID);

	void KillProcessByName(std::string strProcName);

	BOOL KillProcessByPID(UINT nPID);

	BOOL KillProcessByHande(HANDLE hProcess);

private:

	CProcessManagerIMPL *m_pIMPL;	///< 具体进行信息获取实现类。
};

#endif // ProcessM_h__
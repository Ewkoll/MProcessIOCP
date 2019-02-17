/* --------------------------------------------------------------------------------------------*/
//	文件名		:	ProcessManagerIMPL.h
//	目的		:	进程信息获取
//	创建时间	:	2014-11-21 23:40:24
//	创建者		:	IDeath
//
//	历史记录	:	
/* --------------------------------------------------------------------------------------------*/
#ifndef ProcessManagerIMPL_h__
#define ProcessManagerIMPL_h__

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "UserModeNtddk.h"

/**
* @brief 进程信息获取
*/
class CProcessManagerIMPL
{
public:
	/**
	* @brief 创建进程
	*/
	BOOL ICreateProcess(PROCESS_INFORMATION& ProcessInformation, LPSTR lpCommandLine, BOOL bInheritHandles, DWORD dwWaitTime = INFINITE);
	
	/*
	* @brief 创建进程
	*/
	BOOL CreateProcessWithLoginUser(PROCESS_INFORMATION& ProcessInformation, std::string szUserName, std::string szDomain, std::string szPassWord, std::string szApplicationPath, LPSTR lpCommandLine);

	/**
	* @brief 显示进程的访问令牌内容
	*/
	BOOL DisplayCallerAccessTokenInFormation();

	/**
	* @brief 检索用户信息
	*/
	BOOL RetrievtTokenUserData(HANDLE hToken, std::string& strUserName);

	/**
	* @brief 显示进程的访问令牌内容
	*/
	BOOL DisplayTokenInfomation(HANDLE hToken);

	/**
	* @brief 检索进程令牌信息
	*/
	std::shared_ptr<byte> RetrieveTokenInFormationClass(HANDLE hToken, TOKEN_INFORMATION_CLASS nTokenClass, LPDWORD lpdwSize);

	/**
	* @brief 当前进程调整到SE_DEBUG
	*/
	BOOL SetCurrentProcessDebugPrivileges();

	/**
	* @brief 进程权限调整
	*/
	BOOL AdjustProcessPrivileges(HANDLE hToken, LPCTSTR szPrivileges, BOOL bEnable);
	
	/**
	* @brief 枚举进程信息
	*/
	BOOL EnumProcessData();
	
	/**
	* @brief 枚举进程模块
	*/
	BOOL EnumProcessModulesData(DWORD dwProcessID);

	/**
	* @brief 显示进程的内存使用情况
	*/
	BOOL ShowProcessMemoryStatus(DWORD dwProcessID);
	
	/**
	* @brief 显示进程的性能状态
	*/
	BOOL ShowProcessPerformanceStatus();

	/**
	* @brief 获取进程堆信息
	*/
	BOOL ShowProcessHeap();

	/**
	* @brief 获取当前进程ID
	*/
	DWORD GetCurrentProcessID();

	/**
	* @brief 获取当前进程句柄
	*/
	HANDLE GetCurrentProcessHandle(BOOL bRealHandle = FALSE);

	/**
	* @brief 句柄复制
	*/
	BOOL CrossProcessDuplicateHandle(HANDLE hToDuplicated, DWORD dwDestProcessID, HANDLE& hDuplicate);

	/**
	* @brief 关闭复制的句柄
	*/
	BOOL CloseDuplicateHandle(HANDLE hDestProcess, HANDLE hDuplicate);

	/**
	* @brief 是否64位进程
	*/
	BOOL IsWow64Process(UINT nPID);
	
	/**
	* @brief 根据进行名称获取进程ID。
	*/	
	std::vector<DWORD> GetProcessPidByName(LPCTSTR lpszProcName);

	/**
	* @brief 根据进行ID获取进程名称。
	*/
	std::string GetNameByPid(UINT nPID);

	/**
	* @brief 判断进程是否还在运行。
	*/
	bool IsProcessRunning(LPCTSTR lpszProcName, UINT nPID);

	/**
	* @brief 根据名称杀死进程。
	*/
	void KillProcessByName(LPCTSTR lpszProcName);

	/**
	* @brief 根据PID杀死进程。
	*/
	BOOL KillProcessByPID(UINT nPID);

	/**
	* @brief 根据句柄杀死进程。
	*/
	BOOL KillProcessByHande(HANDLE hProcess);

	/**
	* @brief 检查函数是否正常。
	*/
	static BOOL CheckFunctionEntryCode(PBYTE pFunctionAddress);

	/**
	* @brief 错误显示
	*/
	static BOOL DisplayErrorInformation(TCHAR *pszAPI, DWORD dwError);
};

#endif // ProcessManagerIMPL_h__
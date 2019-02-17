#include "stdafx.h"
#include "ProcessM.h"
#include "ProcessManager.h"

CProcessM::CProcessM()
	: m_pIMPL(new CProcessManagerIMPL)
{

}

CProcessM::~CProcessM()
{
	if (NULL != m_pIMPL)
	{
		delete m_pIMPL;
		m_pIMPL = NULL;
	}
}

BOOL CProcessM::ICreateProcess(PROCESS_INFORMATION& ProcessInformation, LPSTR lpCommandLine, BOOL bInheritHandles, DWORD dwWaitTime /*= INFINITE*/)
{
	return m_pIMPL->ICreateProcess(ProcessInformation, lpCommandLine, bInheritHandles, dwWaitTime);
}

BOOL CProcessM::CreateProcessWithLoginUser(PROCESS_INFORMATION& ProcessInformation, std::string szUserName, std::string szDomain, std::string szPassWord, std::string szApplicationPath, LPSTR lpCommandLine)
{
	return m_pIMPL->CreateProcessWithLoginUser(ProcessInformation, szUserName, szDomain, szPassWord, szApplicationPath, lpCommandLine);
}

BOOL CProcessM::SetCurrentProcessDebugPrivileges()
{
	return m_pIMPL->SetCurrentProcessDebugPrivileges();
}

DWORD CProcessM::GetCurrentProcessID()
{
	return m_pIMPL->GetCurrentProcessID();
}

HANDLE CProcessM::GetCurrentProcessHandle(BOOL bRealHandle /*= FALSE*/)
{
	return m_pIMPL->GetCurrentProcessHandle(bRealHandle);
}

BOOL CProcessM::CrossProcessDuplicateHandle(HANDLE hToDuplicated, DWORD dwDestProcessID, HANDLE& hDuplicate)
{
	return m_pIMPL->CrossProcessDuplicateHandle(hToDuplicated, dwDestProcessID, hDuplicate);
}

BOOL CProcessM::CloseDuplicateHandle(HANDLE hDestProcess, HANDLE hDuplicate)
{
	return m_pIMPL->CloseDuplicateHandle(hDestProcess, hDuplicate);
}

BOOL CProcessM::IsWow64Process(UINT nPID)
{
	return m_pIMPL->IsWow64Process(nPID);
}

std::vector<DWORD> CProcessM::GetProcessPidByName(std::string strProcName)
{
	return m_pIMPL->GetProcessPidByName(strProcName.c_str());
}

std::string CProcessM::GetNameByPid(UINT nPID)
{
	return m_pIMPL->GetNameByPid(nPID);
}

bool CProcessM::IsProcessRunning(std::string strProcName, UINT nPID)
{
	return m_pIMPL->IsProcessRunning(strProcName.c_str(), nPID);
}

void CProcessM::KillProcessByName(std::string strProcName)
{
	m_pIMPL->KillProcessByName(strProcName.c_str());
}

BOOL CProcessM::KillProcessByPID(UINT nPID)
{
	return m_pIMPL->KillProcessByPID(nPID);
}

BOOL CProcessM::KillProcessByHande(HANDLE hProcess)
{
	return m_pIMPL->KillProcessByHande(hProcess);
}
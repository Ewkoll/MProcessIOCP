#include "Master.h"
#include <sstream>
#include "ProcessM.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"

CMaster::CMaster(std::string strPath, int nCount)
	: m_strPath(strPath)
	, m_nCount(nCount)
{

}

CMaster::~CMaster()
{

}

bool CMaster::Init(std::string strAddress, int nPort)
{
	bool bResult = false;
	do 
	{
		std::string strErrorMSG;
		if (!m_InitSocket.GetInitResult(strErrorMSG))
		{
			break;
		}

		SOCKET hListenSocket = Listen(strAddress, nPort);
		if (INVALID_SOCKET == hListenSocket)
		{
			break;
		}
		
		CProcessM ProcessM;
		DWORD dwCurrentID = ProcessM.GetCurrentProcessID();
		std::string strMasterName = ProcessM.GetNameByPid(dwCurrentID);
		DebugConsoleLog("Master Name: %s Master PID: %d", strMasterName.c_str(), dwCurrentID);
		
		std::stringstream StringStream;
		StringStream << m_strPath << " " << dwCurrentID << " " << strMasterName;
		std::string strCommandLine = StringStream.str();
		DebugConsoleLog(strCommandLine.c_str());

		for (int nIndex = 0; nIndex < m_nCount; ++nIndex)
		{
			PROCESS_INFORMATION ProcessINFO = { 0 };
			if (!ProcessM.ICreateProcess(ProcessINFO, (LPSTR)strCommandLine.c_str(), TRUE, 100))
			{
				break;
			}

			std::shared_ptr<CWorkerProcessData> Worker(new CWorkerProcessData(dwCurrentID, ProcessINFO.dwProcessId, ProcessINFO));
			if (!Worker->GetIPC().WaitInitFinish())
			{
				ProcessM.KillProcessByPID(ProcessINFO.dwProcessId);
				break;
			}
			
			m_WorkerProcess.push_back(Worker);
		}
		
		for (auto itWork = m_WorkerProcess.begin(); itWork != m_WorkerProcess.end(); ++itWork)
		{
			WSAPROTOCOL_INFO WsaINFO = { 0 };
			if (WSADuplicateSocket(hListenSocket, (*itWork)->GetWorkerPID(), &WsaINFO) == SOCKET_ERROR)
			{
				int nError = GetLastError();
				break;
			}
			
			if (!(*itWork)->GetIPC().SendData(E_IPC_PACKET_TYPE_WSASOCKET_COMMAND, WsaINFO))
			{
				break;
			}
		}
		bResult = true;
	} while (false);

	if (!bResult)
	{
		UnInit();
	}
	return bResult;
}

void CMaster::Run()
{
	bool bRuning = true;
	do 
	{
		int nCount = m_WorkerProcess.size();
		for (auto itWork = m_WorkerProcess.begin(); itWork != m_WorkerProcess.end(); ++itWork)
		{
			if (!(*itWork)->GetIPC().Ping() || !(*itWork)->GetIPC().Pong())
			{
				nCount--;
			}	
		}

		if (nCount == 0)
		{
			break;
		}

		Sleep(5000);
	} while (bRuning);
}

bool CMaster::UnInit()
{
	StopListen();

	CProcessM ProcessM;
	for (auto itWork = m_WorkerProcess.begin(); itWork != m_WorkerProcess.end(); ++itWork)
	{
		if (!(*itWork)->GetIPC().SendExitCommand())
		{
			ProcessM.KillProcessByPID((*itWork)->GetINFO().dwProcessId);
		}
	}
	return true;
}
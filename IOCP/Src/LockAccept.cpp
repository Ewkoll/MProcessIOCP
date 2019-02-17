#include "LockAccept.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"

CLockAccept::CLockAccept(std::string strName /*= std::string()*/)
	: CAcceptBase(strName)
	, m_bRunning(false)
{

}

CLockAccept::~CLockAccept()
{

}

bool CLockAccept::Start()
{
	bool bResult = false;
	do
	{
		if (IsRuning())
		{
			break;
		}

		if (INVALID_SOCKET == m_hListenSocket)
		{
			break;
		}

		m_bRunning = true;
		bResult = CWorker::Start();
	} while (false);
	return bResult;
}

bool CLockAccept::Run()
{
	do
	{
		HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "LockAccept");
		if (NULL == hMutex)
		{
			Sleep(1);
			continue;
		}

		SOCKET hClientSocket = INVALID_SOCKET;
		int nLen = sizeof(SOCKADDR_IN);
		hClientSocket = WSAAccept(m_hListenSocket, NULL, &nLen, ConnectAcceptCondition, (DWORD_PTR)this);
		ReleaseMutex(hMutex);
		if (hClientSocket == SOCKET_ERROR)
		{
			DWORD dwResult = WSAGetLastError();
			if (dwResult != WSAEWOULDBLOCK)
			{
				DebugFileLog(ErrorCode2Text(dwResult).c_str());
			}
		}
		else
		{
			if (NULL == m_pFuncAssociateIncomingClientSocket)
			{
				RejectClientConnect(hClientSocket);
			}
			else
			{
				if (!m_pFuncAssociateIncomingClientSocket(hClientSocket))
				{
					RejectClientConnect(hClientSocket);
				}
			}
		}
	} while (m_bRunning);
	return CWorker::Run();
}

bool CLockAccept::Exit()
{
	m_bRunning = false;
	return CWorker::Exit();
}
#include "Accept.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"

CAccept::CAccept(std::string strName /*= std::string()*/)
	: CAcceptBase(strName)
	, m_bRunning(false)
	, m_hWSAEvent(WSA_INVALID_EVENT)
{

}

CAccept::~CAccept()
{
	StopWaitEvent();
}

bool CAccept::Start()
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

		if (WSA_INVALID_EVENT == m_hWSAEvent)
		{
			m_hWSAEvent = WSACreateEvent();
		}
		
		if (WSA_INVALID_EVENT == m_hWSAEvent)
		{	
			break;
		}
		
		if (SOCKET_ERROR == WSAEventSelect(m_hListenSocket, m_hWSAEvent, FD_ACCEPT | FD_CLOSE))
		{
			DebugConsoleLog("WSAEventSelect Failed [%d]...", m_hListenSocket);
			StopWaitEvent();
			break;
		}

		m_bRunning = true;
		bResult = CWorker::Start();
	} while (false);

	if (!bResult)
	{
		DebugFileLog(ErrorCode2Text(GetLastError()).c_str());
	}
	return bResult;
}

bool CAccept::Run()
{
	LINGER soLINGER = { TRUE, 30 };
	BOOL bNodelay = TRUE;
	do
	{
		// 等待事件。
		DWORD dwResult = WaitForSingleObject(m_hWSAEvent, 500);
		if (dwResult == WAIT_FAILED)
		{
			break;
		}
		
		// 判断是否要退出线程。
		if (!m_bRunning)
		{
			break;
		}

		// 超时继续等待。
		if (dwResult == WSA_WAIT_TIMEOUT)
		{
			continue;
		}

		// 判断事件类型，如果是CLOSE监听，退出循环线程。
		WSANETWORKEVENTS WorkEvent;
		dwResult = ::WSAEnumNetworkEvents(m_hListenSocket, m_hWSAEvent, &WorkEvent);
		if (dwResult == SOCKET_ERROR)
		{
			DebugFileLog(ErrorCode2Text(GetLastError()).c_str());
			break;
		}

		// 监听CLOSE，那么关闭Accept线程。
		if (WorkEvent.lNetworkEvents & FD_CLOSE)
		{
			break;
		}
		
		// 判断是否是ACCEPT事件。
		BOOL bAcceptEvent = WorkEvent.lNetworkEvents & FD_ACCEPT;
		if (!bAcceptEvent)
		{
			continue;
		}

		// 判断是否允许接受新连接。
		if (!m_bAcceptConnections)
		{
			continue;
		}
		
		SOCKET hClientSocket = INVALID_SOCKET;
		int nLen = sizeof(SOCKADDR_IN);
		hClientSocket = WSAAccept(m_hListenSocket, NULL, &nLen, ConnectAcceptCondition, (DWORD_PTR)this);
		if (hClientSocket == SOCKET_ERROR)
		{
			dwResult = WSAGetLastError();
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
	} while (true);
	return CWorker::Run();
}

bool CAccept::Exit()
{
	m_bRunning = false;
	StopWaitEvent();
	return CWorker::Exit();
}
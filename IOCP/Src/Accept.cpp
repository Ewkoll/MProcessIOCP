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
		// �ȴ��¼���
		DWORD dwResult = WaitForSingleObject(m_hWSAEvent, 500);
		if (dwResult == WAIT_FAILED)
		{
			break;
		}
		
		// �ж��Ƿ�Ҫ�˳��̡߳�
		if (!m_bRunning)
		{
			break;
		}

		// ��ʱ�����ȴ���
		if (dwResult == WSA_WAIT_TIMEOUT)
		{
			continue;
		}

		// �ж��¼����ͣ������CLOSE�������˳�ѭ���̡߳�
		WSANETWORKEVENTS WorkEvent;
		dwResult = ::WSAEnumNetworkEvents(m_hListenSocket, m_hWSAEvent, &WorkEvent);
		if (dwResult == SOCKET_ERROR)
		{
			DebugFileLog(ErrorCode2Text(GetLastError()).c_str());
			break;
		}

		// ����CLOSE����ô�ر�Accept�̡߳�
		if (WorkEvent.lNetworkEvents & FD_CLOSE)
		{
			break;
		}
		
		// �ж��Ƿ���ACCEPT�¼���
		BOOL bAcceptEvent = WorkEvent.lNetworkEvents & FD_ACCEPT;
		if (!bAcceptEvent)
		{
			continue;
		}

		// �ж��Ƿ�������������ӡ�
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
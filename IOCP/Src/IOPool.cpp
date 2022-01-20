#include "IOPool.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"

using namespace IOCP;
#define NOTIFY_EXIT_COMPLETIONKEY ((ULONG_PTR)-1)

CIOPool::CIOPool(DWORD dwIOWorker)
	: m_dwIOWorker(dwIOWorker)
	, m_hCompletionPort(INVALID_HANDLE_VALUE)
{
	
}

CIOPool::~CIOPool()
{
	Destroy();
}

bool CIOPool::Associate(HANDLE hSocket, ULONG_PTR dwCompletionKey /* = 0 */) const
{
	HANDLE hBindPort = CreateIoCompletionPort(hSocket, m_hCompletionPort, dwCompletionKey, 0);
	return hBindPort == m_hCompletionPort;
}

bool CIOPool::Start()
{
	HANDLE hCompletionPort(INVALID_HANDLE_VALUE);
	bool bResult = false;
	do 
	{
		std::string strErrorMSG;
		if (!m_InitSocket.GetInitResult(strErrorMSG))
		{
			break;
		}

		hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_dwIOWorker);
		if (NULL == hCompletionPort)
		{
			break;
		}

		m_hCompletionPort = hCompletionPort;
		for (DWORD nIndex = 0; nIndex < m_dwIOWorker * 2; ++nIndex)
		{
			m_vIOWorkerThread.push_back(std::move(std::thread(IOCPWorkerThread, m_hCompletionPort)));
		}
		bResult = true;
	} while (false);
	return bResult;
}

bool CIOPool::Destroy()
{
	for (auto& it : m_vIOWorkerThread)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, NOTIFY_EXIT_COMPLETIONKEY, 0);
	}
	
	for (auto& it : m_vIOWorkerThread)
	{
		if (it.joinable())
		{
			it.join();
		}
	}

	m_vIOWorkerThread.clear();
	if (m_hCompletionPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = INVALID_HANDLE_VALUE;
	}
	return true;
}

void CIOPool::IOCPWorkerThread(HANDLE hCompletionPort)
{
	ULONG_PTR dwCompletionKey = 0;
	DWORD dwIoSize = 0;
	while (true)
	{
		LPOVERLAPPED IpOverlapped = NULL;
		BOOL bResult = GetQueuedCompletionStatus(hCompletionPort,
												 &dwIoSize,
												 (PULONG_PTR)&dwCompletionKey,
												 (LPOVERLAPPED*)&IpOverlapped,
												 INFINITE);
		if (dwCompletionKey == (ULONG_PTR)NOTIFY_EXIT_COMPLETIONKEY)
		{
			InfoConsoleLog("���յ��˳�IOCP�¼�");
			break;
		}

		int nErrorCode = GetLastError();
		if (!bResult)
		{
			if (nErrorCode == ERROR_ABANDONED_WAIT_0)
			{
				InfoConsoleLog("ERROR_ABANDONED_WAIT_0 ��������ɶ˿��Ѿ��رգ��������������˳�! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			}

			switch (nErrorCode)
			{
			case ERROR_CONNECTION_ABORTED:
				DebugFileLog("ERROR_CONNECTION_ABORTED: �ڲ��׽��ֹرյ��µ������ж�! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			case ERROR_NETNAME_DELETED:
				DebugFileLog("ERROR_NETNAME_DELETED: ������ͨ���׽��ֱ�ȡ������PENDING��WSASend/WSARead��δ���! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			case ERROR_OPERATION_ABORTED:
				DebugFileLog("ERROR_OPERATION_ABORTED:IOCP�л���WSARecv�Ļ���IO���������Ƕ�Ӧ���׽����Ѿ��رջᱨ�������! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			default:
				DebugFileLog(ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		
		OVERLAPPEDEX *pOvlEx = CONTAINING_RECORD(IpOverlapped, OVERLAPPEDEX, m_Overlapped);
		if (NULL == pOvlEx)
		{
			DebugFileLog("�����ϲ�Ӧ�÷����Ĵ���! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
			continue;
		}

		if (NULL == pOvlEx->m_pPoolClient)
		{
			DebugFileLog("û������Pool�ͻ��ˣ����ش���");
			break;
		}

		if (bResult)
		{
			pOvlEx->m_pPoolClient->OnOperationCompleted(IpOverlapped, dwIoSize, dwCompletionKey);
		}
		else
		{
			pOvlEx->m_pPoolClient->OnOperationCanceled(IpOverlapped, nErrorCode, dwIoSize, dwCompletionKey);
		}
	}
}
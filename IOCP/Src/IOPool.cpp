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
			InfoConsoleLog("接收到退出IOCP事件");
			break;
		}

		int nErrorCode = GetLastError();
		if (!bResult)
		{
			if (nErrorCode == ERROR_ABANDONED_WAIT_0)
			{
				InfoConsoleLog("ERROR_ABANDONED_WAIT_0 关联的完成端口已经关闭，程序流程让其退出! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			}

			switch (nErrorCode)
			{
			case ERROR_CONNECTION_ABORTED:
				DebugFileLog("ERROR_CONNECTION_ABORTED: 内部套接字关闭导致的连接中断! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			case ERROR_NETNAME_DELETED:
				DebugFileLog("ERROR_NETNAME_DELETED: 发生在通信套接字被取消并且PENDING的WSASend/WSARead还未完成! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			case ERROR_OPERATION_ABORTED:
				DebugFileLog("ERROR_OPERATION_ABORTED:IOCP中还在WSARecv的或者IO操作，但是对应的套接字已经关闭会报这个错误! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
				break;
			default:
				DebugFileLog(ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		
		OVERLAPPEDEX *pOvlEx = CONTAINING_RECORD(IpOverlapped, OVERLAPPEDEX, m_Overlapped);
		if (NULL == pOvlEx)
		{
			DebugFileLog("理论上不应该发生的错误! MSG=%s", ErrorCode2Text(nErrorCode).c_str());
			continue;
		}

		if (NULL == pOvlEx->m_pPoolClient)
		{
			DebugFileLog("没有设置Pool客户端，严重错误！");
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
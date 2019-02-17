#include "CommunicationWorker.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"
#include "AsyncOperator.h"
#include "ProtocolAnalysis.h"

using namespace IOCP;

void CTCPWorker::OnOperationCompleted(LPOVERLAPPED pOVL, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey)
{
	CIOCPBuffer *pIOCPBuffer = CONTAINING_RECORD(pOVL, CIOCPBuffer, m_Overlapped);
	if (NULL == pIOCPBuffer)
	{
		return;
	}

	ProcessIOMessage(pIOCPBuffer, dwTransferred, dwCompletionKey);
}

void CTCPWorker::OnOperationCanceled(LPOVERLAPPED pOVL, int nErrorCode, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey)
{
	CIOCPBuffer *pIOCPBuffer = CONTAINING_RECORD(pOVL, CIOCPBuffer, m_Overlapped);
	if (NULL == pIOCPBuffer)
	{
		return;
	}
	
	CTcpClientManager::GetInstance().RemoveClient((SOCKET)dwCompletionKey);
	CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
}

void CTCPWorker::ProcessIOMessage(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, ULONG_PTR dwCompletionKey)
{
	std::shared_ptr<CClientContext> pContext = CTcpClientManager::GetInstance().GetClient((SOCKET)dwCompletionKey);
	if (NULL == pContext)
	{
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
		return;
	}
	
	OPERATION_TYPE nType = pIOCPBuffer->GetOperation();
	pContext->UpdateTime();
	switch (nType)
	{
	case OP_IOInitialize:
		OnInitialize(pIOCPBuffer, dwSize, pContext);
		break;
	case OP_IORead:
		OnRead(pIOCPBuffer, dwSize, pContext);
		break;
	case OP_IOReadCompleted:
		OnReadCompleted(pIOCPBuffer, dwSize, pContext);
		break;
	case OP_IOWrite:
		OnWrite(pIOCPBuffer, dwSize, pContext);
		break;
	case OP_IOWriteCompleted:
		OnWriteCompleted(pIOCPBuffer, dwSize, pContext);
		break;
	}
}

void CTCPWorker::OnInitialize(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext)
{
	DoRead(pContext, pIOCPBuffer);
}

void CTCPWorker::OnRead(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext)
{
	SOCKET hSocket = pContext->GetSocket();
	bool bResult = false;
	do
	{
		if (hSocket == INVALID_SOCKET)
		{
			DebugFileLog("OnRead-套接字没有设置-理论上不可能到的逻辑！");
			break;
		}

		HANDLE hCompletionPort = pContext->GetCompletionPort();
		if (INVALID_HANDLE_VALUE == hCompletionPort)
		{
			DebugFileLog("OnRead-完成端口没有设置-理论上不可能到的逻辑！");
			break;
		}

		if (NULL == pIOCPBuffer->GetPoolClient())
		{
			DebugFileLog("OnRead-复用来至于DoRead投递的缓存区间，关联PoolClient不可能为空！");
			break;
		}

		pIOCPBuffer->SetOperation(OP_IOReadCompleted);
		pIOCPBuffer->SetupRead();
		ULONG ulFlags = MSG_PARTIAL;
		DWORD dwIoSize = 0;
		UINT nResult = WSARecv(hSocket,
							   pIOCPBuffer->GetWSABuffer(),
							   1,
							   &dwIoSize,
							   &ulFlags,
							   &pIOCPBuffer->m_Overlapped,
							   NULL);
		if (nResult == SOCKET_ERROR)
		{
			INT nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
			{
				DebugFileLog("OnRead-WSARecv Failed %s", ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		bResult = true;
	} while (false);

	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(hSocket);
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
	}
}

void CTCPWorker::OnReadCompleted(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext)
{
	if (0 == dwSize)
	{
		DebugFileLog("Disconnection OnReadCompleted Socket=[%d]", pContext->GetSocket());
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
		return;
	}

	pContext->AdjustReadSequenceNumber();
	pIOCPBuffer->Use(dwSize);

	if (!CProtocolAnalysis::GetInstance().ParseReceivePackage(pContext->GetProtocolType(), pIOCPBuffer, dwSize, pContext, m_CallBack))
	{
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
	}
}

bool CTCPWorker::ReadCompletedCallBack(E_RECEIVE_PKG_STATUS status, CIOCPBuffer* pIOCPBuffer, std::shared_ptr<CClientContext>& pContext)
{
	bool bResult = false;
	do
	{
		if (NULL == pIOCPBuffer || NULL == pContext)
		{
			break;
		}

		switch (status)
		{
			case IOCP::E_RECEIVE_PKG_FINISHED:
			{
				bResult = DoRead(pContext, pIOCPBuffer);
			}
			break;
			case IOCP::E_RECEIVE_PKG_UNFINISHED:
			{
				bResult = CAsyncOperator::ContinueAsyncRead(pContext, pIOCPBuffer);
			}
			break;
		}
	} while (false);
	return bResult;
}

void CTCPWorker::OnWrite(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext)
{
	SOCKET hSocket = pContext->GetSocket();
	bool bResult = false;
	do
	{
		if (hSocket == INVALID_SOCKET)
		{
			DebugFileLog("OnWrite-套接字没有设置-理论上不可能到的逻辑！");
			break;
		}

		HANDLE hCompletionPort = pContext->GetCompletionPort();
		if (INVALID_HANDLE_VALUE == hCompletionPort)
		{
			DebugFileLog("OnWrite-完成端口没有设置-理论上不可能到的逻辑！");
			break;
		}

		if (NULL == pIOCPBuffer || NULL == pIOCPBuffer->GetPoolClient())
		{
			DebugFileLog("OnWrite-复用来至于AsyncSend投递的缓存区间，关联PoolClient不可能为空！");
			break;	
		}

		pIOCPBuffer->SetOperation(OP_IOWriteCompleted);
		pContext->AddWriteQueue();
		ULONG ulFlags = MSG_PARTIAL;
		DWORD dwSendNumBytes = 0;
		UINT nResult = WSASend(hSocket,
							   pIOCPBuffer->GetWSABuffer(),
							   1,
							   &dwSendNumBytes,
							   ulFlags,
							   &pIOCPBuffer->m_Overlapped,
							   NULL);
		if (nResult == SOCKET_ERROR)
		{
			INT nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
			{
				DebugFileLog("OnWrite-WSASend Failed[%s]", ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		bResult = true;
	} while (false);

	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(hSocket);
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
	}
}

void CTCPWorker::OnWriteCompleted(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext)
{
	if (0 == dwSize)
	{
		DebugFileLog("Disconnection OnWriteCompleted Socket=[%d]", pContext->GetSocket());
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
		return;
	}
	
	pContext->AdjustSendSequenceNumber();
	pContext->ReleaseWriteQueue();
	bool bResult = false;
	if (dwSize < pIOCPBuffer->GetUsed())
	{
		bResult = CAsyncOperator::ContinueAsyncSend(pContext, dwSize, pIOCPBuffer);
	}
	else
	{
		bResult = CProtocolAnalysis::GetInstance().NotofySendCompleted(pContext->GetProtocolType(), pIOCPBuffer, pContext);
		if (bResult)
		{
			CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
		}
	}

	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
	}
}

bool CTCPWorker::DoRead(std::shared_ptr<CClientContext>& pContext, CIOCPBuffer *pIOCPBuffer /*= NULL*/)
{
	SOCKET hSocket = pContext->GetSocket();
	bool bResult = false;
	do
	{
		if (hSocket == INVALID_SOCKET)
		{
			DebugFileLog("套接字没有设置-理论上不可能到的逻辑！");
			break;
		}

		HANDLE hCompletionPort = pContext->GetCompletionPort();
		if (hCompletionPort == INVALID_HANDLE_VALUE)
		{
			DebugFileLog("完成端口没有设置-理论上不可能到的逻辑！");
			break;
		}
		
		if (NULL == pIOCPBuffer)
		{
			pIOCPBuffer = CTCPBufferManager::GetInstance().AllocateBuffer(OP_IORead);
			if (NULL == pIOCPBuffer)
			{
				DebugFileLog("获取BUFFER失败-理论上不可能到的逻辑！");
				break;
			}
		}
		
		pIOCPBuffer->SetOperation(OP_IORead);
		pIOCPBuffer->SetPoolClient(&CTCPWorker::GetInstance());
		if (!pIOCPBuffer->SetupRead())
		{
			break;
		}
		
		bool bSuccess = (PostQueuedCompletionStatus(hCompletionPort, 0, (ULONG_PTR)hSocket, &pIOCPBuffer->m_Overlapped) == TRUE);
		if (!bSuccess)
		{
			INT nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
			{
				DebugFileLog("DoRead - PostQueuedCompletionStatus Failed[%s]", ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		bResult = true;
	} while (false);

	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(hSocket);
		CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
	}
	return bResult;
}
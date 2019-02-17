#include "AsyncOperator.h"
#include "DebugLog.h"
#include "ErrorCodeMSG.h"
#include "CommunicationWorker.h"

bool IOCP::CAsyncOperator::AsyncSend(std::shared_ptr<CClientContext>& pContext, void *pData, int nDataLen, IExecutor* pExecutor)
{
	if (NULL == pContext)
	{
		return false;
	}

	pContext->CheckWriteQueueStatus();
	bool bResult = IOCP::CAsyncOperator::AsyncSend(pContext->GetCompletionPort(), pContext->GetSocket(), pData, nDataLen, pExecutor, pContext->GetSocket());
	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
	}
	return bResult;
}

bool IOCP::CAsyncOperator::AsyncSend(HANDLE hCompletionPort, SOCKET hSocket, void *pData, int nDataLen, IExecutor* pExecutor, ULONG_PTR dwCompletionKey)
{
	CIOCPBuffer *pBuffer = NULL;
	bool bResult = false;
	do 
	{
		if (INVALID_HANDLE_VALUE == hCompletionPort || NULL == pData || NULL == pExecutor)
		{
			break;
		}

		pBuffer = CTCPBufferManager::GetInstance().AllocateBuffer(OP_IOWrite, nDataLen);
		if (NULL == pBuffer)
		{
			break;
		}

		if (!pBuffer->SetData((char*)pData, nDataLen))
		{
			break;
		}
		
		pBuffer->SetupWrite();
		pBuffer->SetPoolClient(pExecutor);

		bool bSuccess = (PostQueuedCompletionStatus(hCompletionPort, pBuffer->GetUsed(), dwCompletionKey, &pBuffer->m_Overlapped) == TRUE);
		if (!bSuccess)
		{
			int nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
			{
				DebugFileLog("AsyncSend Failed![%s]", ErrorCode2Text(nErrorCode).c_str());
				break;
			}
		}
		bResult = true;
	} while (false);

	if (!bResult && NULL != pBuffer)
	{
		CTCPBufferManager::GetInstance().ReleaseBuffer(pBuffer);
	}
	return bResult;
}

bool IOCP::CAsyncOperator::ContinueAsyncSend(std::shared_ptr<CClientContext>& pContext, int dwBytesTransferred, CIOCPBuffer *pBuffer)
{
	if (NULL == pContext)
	{
		return false;
	}
	
	bool bResult = IOCP::CAsyncOperator::ContinueAsyncSend(pContext->GetCompletionPort(), dwBytesTransferred, pBuffer, pContext->GetSocket());
	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
	}
	return bResult;
}

bool IOCP::CAsyncOperator::ContinueAsyncSend(HANDLE hCompletionPort, int dwBytesTransferred, CIOCPBuffer *pBuffer, ULONG_PTR dwCompletionKey)
{
	if (INVALID_HANDLE_VALUE == hCompletionPort || NULL == pBuffer)
	{
		return false;
	}

	pBuffer->SetOperation(OP_IOWrite);
	if (!pBuffer->SetupWrite(dwBytesTransferred))
	{
		return false;
	}
	
	bool bSuccess = (PostQueuedCompletionStatus(hCompletionPort, pBuffer->GetUsed(), dwCompletionKey, &pBuffer->m_Overlapped) == TRUE);
	if (!bSuccess)
	{
		int nErrorCode = GetLastError();
		if (nErrorCode != ERROR_IO_PENDING)
		{
			DebugFileLog("ContinueAsyncSend Failed![%s]", ErrorCode2Text(nErrorCode).c_str());
			return false;
		}
	}
	return true;
}

bool IOCP::CAsyncOperator::ContinueAsyncRead(std::shared_ptr<CClientContext>& pContext, CIOCPBuffer *pBuffer, unsigned int nReadLength /*= INVALID_LENGTH*/)
{
	if (NULL == pContext)
	{
		return false;
	}

	bool bResult = IOCP::CAsyncOperator::ContinueAsyncRead(pContext->GetCompletionPort(), pBuffer, pContext->GetSocket(), nReadLength);
	if (!bResult)
	{
		CTcpClientManager::GetInstance().RemoveClient(pContext->GetSocket());
	}
	return bResult;
}

bool IOCP::CAsyncOperator::ContinueAsyncRead(HANDLE hCompletionPort, CIOCPBuffer *pBuffer, ULONG_PTR dwCompletionKey, unsigned int nReadLength /*= INVALID_LENGTH*/)
{
	if (INVALID_HANDLE_VALUE == hCompletionPort || NULL == pBuffer)
	{
		return false;
	}

	pBuffer->SetOperation(OP_IORead);
	if (!pBuffer->SetupRead(nReadLength))
	{
		return false;
	}

	bool bSuccess = (PostQueuedCompletionStatus(hCompletionPort, pBuffer->GetUsed(), dwCompletionKey, &pBuffer->m_Overlapped) == TRUE);
	if (!bSuccess)
	{
		int nErrorCode = GetLastError();
		if (nErrorCode != ERROR_IO_PENDING)
		{
			DebugFileLog("ContinueAsyncRead Failed![%s]", ErrorCode2Text(nErrorCode).c_str());
			return false;
		}
	}
	return true;
}
/*****************************************************************************
*  @file     AsyncOperator.h
*  @brief    Òì²½²Ù×÷¡£
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 10:55:28
*****************************************************************************/
#ifndef AsyncOperator_h__
#define AsyncOperator_h__

#pragma once
#include "IOCPBuffer.h"
#include "ClientManager.h"

namespace IOCP
{
	class CAsyncOperator
	{
	public:
		static bool AsyncSend(std::shared_ptr<CClientContext>& pContext, void *pData, int nDataLen, IExecutor* pExecutor);

		static bool ContinueAsyncSend(std::shared_ptr<CClientContext>& pContext, int dwBytesTransferred, CIOCPBuffer *pBuffer);

		static bool ContinueAsyncRead(std::shared_ptr<CClientContext>& pContext, CIOCPBuffer *pBuffer, unsigned int nReadLength = INVALID_LENGTH);

	private:

		static bool AsyncSend(HANDLE hCompletionPort, SOCKET hSocket, void *pData, int nDataLen, IExecutor* pExecutor, ULONG_PTR dwCompletionKey);

		static bool ContinueAsyncSend(HANDLE hCompletionPort, int dwBytesTransferred, CIOCPBuffer *pBuffer, ULONG_PTR dwCompletionKey);

		static bool ContinueAsyncRead(HANDLE hCompletionPort, CIOCPBuffer *pBuffer, ULONG_PTR dwCompletionKey, unsigned int nReadLength = INVALID_LENGTH);
	};
}

#endif // AsyncOperator_h__
/*****************************************************************************
*  @file     CommunicationWorker.h
*  @brief    通信工作对象。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-03 10:09:24
*****************************************************************************/
#ifndef CommunicationWorker_h__
#define CommunicationWorker_h__

#pragma once
#include "IOPool.h"
#include "IOCPBuffer.h"
#include "ClientManager.h"
#include "IOCPBufferManager.h"
#include "ProtocolAnalysis.h"

namespace IOCP
{
	/*
	*	TCP协议的客户端管理器。
	*/
	class CTcpClientManager	: public CClientManager 
							, public Singleton<CTcpClientManager>
	{
	public:		

		CTcpClientManager(UINT nMaxClientCount = E_MAX_CLIENT_COUNT)
			: m_nMaxClientCount(nMaxClientCount)
		{

		}

		enum { E_MAX_CLIENT_COUNT = 30000 };
		
		bool IsAccept() { return GetSize() <= m_nMaxClientCount; };

	private:

		UINT m_nMaxClientCount;				///< 最大允许的客户端连接数量。
	};
	
	/*
	*	TCP缓冲区的管理器。
	*/
	SingletonClass(CTCPBufferManager, CIOCPBufferManager)
	
	/*
	*	自定义数据包TCP协议处理过程。
	*/
	class CTCPWorker : public IExecutor
					 , public Singleton<CTCPWorker>
	{
	public:
		
		CTCPWorker()
		{
			m_CallBack = std::bind(&CTCPWorker::ReadCompletedCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}

		virtual void OnOperationCompleted(LPOVERLAPPED pOVL, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey) override;

		virtual void OnOperationCanceled(LPOVERLAPPED pOVL, int nErrorCode, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey) override;

	protected:

		virtual void ProcessIOMessage(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, ULONG_PTR dwCompletionKey);

		inline void OnInitialize(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext);

		inline void OnRead(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext);

		inline void OnReadCompleted(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext);

		inline void OnWrite(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext);

		inline void OnWriteCompleted(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext);

	protected:

		bool ReadCompletedCallBack(E_RECEIVE_PKG_STATUS status, CIOCPBuffer* pIOBuffer, std::shared_ptr<CClientContext>& pContext);

		inline bool DoRead(std::shared_ptr<CClientContext>& pContext, CIOCPBuffer *pIOCPBuffer = NULL);
		
	protected:

		ReceivePackageCallBack m_CallBack;
	};
}

#endif // CommunicationWorker_h__

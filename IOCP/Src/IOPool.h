/*****************************************************************************
*  @file     IOPool.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-02 17:05:21
*****************************************************************************/
#pragma once
#include <thread>
#include <vector>
#include "InitSocket.h"
#include "Common/Global.h"

namespace IOCP
{
	class IExecutor
	{
	public:
		virtual ~IExecutor() {}
		virtual void OnOperationCompleted(LPOVERLAPPED pOVL, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey) = 0;
		virtual void OnOperationCanceled(LPOVERLAPPED pOVL, int nErrorCode, const DWORD dwTransferred, const ULONG_PTR dwCompletionKey) = 0;
	};

	class OVERLAPPEDEX
	{
	public:
		OVERLAPPED m_Overlapped;
		IExecutor* m_pPoolClient;

		explicit OVERLAPPEDEX(IExecutor* pPoolClient = NULL)
			: m_pPoolClient(pPoolClient)
		{
			ZEROMEMORY(&m_Overlapped, sizeof(m_Overlapped));
		}
	};

	class CIOPool
	{
	private:
		
		CIOPool(const CIOPool& Other) = delete;

		CIOPool& operator=(const CIOPool& Other) = delete;

	public:

		explicit CIOPool(DWORD dwIOWorker);
		
		~CIOPool();

		bool Associate(HANDLE hSocket, ULONG_PTR dwCompletionKey = 0) const;

		bool Start();

		bool Destroy();

		HANDLE& GetCompletionPort() { return m_hCompletionPort; };

	private:

		static void IOCPWorkerThread(HANDLE hCompletionPort);
		
	private:
		std::vector<std::thread> m_vIOWorkerThread;					///< 工作线程。
		DWORD m_dwIOWorker;											///< 线程数量。
		HANDLE m_hCompletionPort;									///< 完成端口。
		CInitSocket m_InitSocket;									///< 初始化网络库。
	};
}
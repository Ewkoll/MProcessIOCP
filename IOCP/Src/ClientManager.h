/*****************************************************************************
*  @file     ClientManager.h
*  @brief    客户端连接管理，确定链接数。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/03/21 10:26:49
*****************************************************************************/
#ifndef ClientManager_h__
#define ClientManager_h__

#pragma once
#include <Winsock2.h>
#include <Windows.h>
#include <functional>
#include <atomic>
#include <string>
#include "Lock.h"
#include "IOCPBuffer.h"
#include "DebugLog.h"
#include "Common/Sington.h"
#include "HashMap/HashMap.h"
#include <MSWSock.h>
using namespace HashMAP;
using namespace HashList;

namespace IOCP
{
	/*
	*	客户端线程上下文。
	*/
	class CClientContext
	{
	public:
		CClientContext()
		{
			Init();
		}

		CClientContext(SOCKET hSocket, const std::string& strClientAddress)
		{
			Init();
			m_hClientSocket = hSocket;
			m_strClientAddress = strClientAddress;
		}

		~CClientContext()
		{
			DebugFileLog("~CClientContext() %d", m_hClientSocket);
		}

		void Init()
		{
			m_hClientSocket					= INVALID_SOCKET;
			m_hCompletionPort				= INVALID_HANDLE_VALUE;
			UpdateTime();
			m_nWriteQueueSize				= 0;
			m_nSendSequenceNumber			= 0;
			m_nReadSequenceNumber			= 0;
			m_nProtocolType					= 0;
		}

		enum 
		{
			WRITE_QUEUE_MAX_WAIT_SIZE = 1024,
			CONNECT_NOREQUEST_EXPIRED_TIME = 60,
		};

		inline SOCKET GetSocket()
		{
			return m_hClientSocket;
		}

		inline std::string& GetClientAddress()
		{
			return m_strClientAddress;
		}

		inline int GetConnectTime()
		{
			int  nSecs;
			int   nBytes = sizeof(int);
			getsockopt(GetSocket(), SOL_SOCKET, SO_CONNECT_TIME, (char*)&nSecs, &nBytes);
			return nSecs;
		}

		inline void AddWriteQueue()
		{
			m_nWriteQueueSize++;
		}
		
		inline void ReleaseWriteQueue()
		{
			m_nWriteQueueSize--;
		}

		inline void CheckWriteQueueStatus()
		{
			if (m_nWriteQueueSize > WRITE_QUEUE_MAX_WAIT_SIZE)
			{
				Sleep(m_nWriteQueueSize/ WRITE_QUEUE_MAX_WAIT_SIZE + 1);
			}
		}

		/////////////////////////////////////////////////////////////////////
		// 写入序列
		/////////////////////////////////////////////////////////////////////

		inline UINT GetSendSequenceNumber()
		{
			return m_nSendSequenceNumber;
		}

		inline void SetSendSequenceNumber(UINT nSeqNumber)
		{
			m_nSendSequenceNumber = nSeqNumber;
		}

		inline void AdjustSendSequenceNumber()
		{
			++m_nSendSequenceNumber;
		}

		/////////////////////////////////////////////////////////////////////
		// 读取序列
		/////////////////////////////////////////////////////////////////////

		inline UINT GetReadSequenceNumber()
		{
			return m_nReadSequenceNumber;
		}
	
		inline void SetReadSequenceNumber(UINT nSeqNumber)
		{
			m_nReadSequenceNumber = nSeqNumber;
		}

		inline void AdjustReadSequenceNumber()
		{
			++m_nReadSequenceNumber;
		}

		inline void SetCompletionPort(HANDLE hCompletionPort)
		{
			m_hCompletionPort = hCompletionPort;
		}
		
		HANDLE GetCompletionPort()
		{
			return m_hCompletionPort;
		}

		inline void UpdateTime()
		{
			m_dwLastTime = GetTickCount64();
		}

		inline bool IsExpired()
		{
			return (GetTickCount64() - m_dwLastTime) / 1000 > CONNECT_NOREQUEST_EXPIRED_TIME;
		}

		int GetProtocolType() { return m_nProtocolType; };

		void SetProtocolType(int nProtocolType) { m_nProtocolType = nProtocolType; }

	protected:
		std::atomic<UINT64> m_dwLastTime;					///< 上次的请求时间。
		int m_nProtocolType;								///< 协议类型。
		HANDLE m_hCompletionPort;							///< 完成端口句柄。
		SOCKET m_hClientSocket;								///< 套接字信息。
		std::string m_strClientAddress;						///< 客户端地址。
		std::atomic<UINT> m_nWriteQueueSize;				///< 发送数据序号。

	protected:
		std::atomic<UINT> m_nSendSequenceNumber;			///< 发送数据序号。
		std::atomic<UINT> m_nReadSequenceNumber;			///< 读取数据序号。
	};

	#define CLIENT_HASHMAP_INITSIZE 4096

	/*
	 *	客户端管理。
	 */
	class CClientManager
	{
		typedef std::function<void(std::shared_ptr<CClientContext>& pClientConect, bool bNewClient)> NotifyCallBack;

	public:

		CClientManager() : m_pCallBack(NULL), m_mapClientContext(HashMAP::HashValue) {};

		bool AddClient(SOCKET hClientSocket, HANDLE hCompletionPort);

		bool AddClient(SOCKET hClientSocket, const std::string& strClientAddress, HANDLE hCompletionPort);

		bool RemoveClient(SOCKET hClientSocket);

		bool DisconnectClientContext(std::shared_ptr<CClientContext>& pContext);

		size_t GetSize();

		bool DisConnectClient(SOCKET hSocket);

		void UpdateClientTime(SOCKET hClientSocket);

		void Vistor(IVistor<std::shared_ptr<CClientContext>, SOCKET>& Vist);

		void SetNotifyCallBack(NotifyCallBack pCallBack);
		
		std::shared_ptr<CClientContext> GetClient(SOCKET hClientSocket);

	private:

		std::string GetHostAddress(SOCKET hClientSocket);
		
	private:
		NotifyCallBack m_pCallBack;
		CHashMap<std::shared_ptr<CClientContext>, SOCKET, CLIENT_HASHMAP_INITSIZE> m_mapClientContext;
	};
}

#endif // ClientManager_h__
/*****************************************************************************
*  @file     TCPServer.h
*  @brief	 
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-03 09:28:20
*****************************************************************************/
#ifndef TCPServer_h__
#define TCPServer_h__

#pragma once
#include "IOPool.h"
#include "AcceptBase.h"
#include "IServer.h"
#include "SystemINFO.h"
#include "ProtocolAnalysis.h"
#include "ConnectionCheck.h"

namespace IOCP
{
	class CTCPServer : public IServer
	{
	public:

		CTCPServer(bool bProcessMode = false, DWORD dwIOWorker = CSystemINFO::GetNumberOfProcessors());

		virtual ~CTCPServer();

		void SetListenSocket(SOCKET hListenSocket);

		std::string GetHostIP();

		unsigned int GetConnectCount();

		void AddBannedIP(SOCKADDR_IN& SockAddr);

		void RemoveBannedIP(SOCKADDR_IN& SockAddr);

		void SetAcceptConnections(bool bAcceptConnections);

	public:

		virtual bool Start() override;
		
		virtual bool Stop() override;
		
		virtual void Wait(unsigned long dwMilliseconds) override;
		
		virtual bool IsRuning() override;

		virtual int GetProtocolType() { return CProtocolAnalysis::E_SUPPORT_ANALYSIS_COMMON; };
	
	private:
		
		void DisableNagle(SOCKET hClientSocket);

		virtual bool AssociateIncomingClientSocket(SOCKET& hSocket);

	private:
		std::shared_ptr<CConnectionCheck> m_pConnectCheck;	///< 连接检查。
		CIOPool m_Pool;										///< IOPool。
		std::shared_ptr<CAcceptBase> m_pAcceptConnect;		///< 接受连接线程。
	};
}

#endif // TCPServer_h__
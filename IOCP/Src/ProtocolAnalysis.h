/*****************************************************************************
*  @file     ProtocolAnalysis.h
*  @brief    协议解析，负责协议包解析，决定收发包是否完整。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 10:02:53
*****************************************************************************/
#ifndef ProtocolAnalysis_h__
#define ProtocolAnalysis_h__

#pragma once
#include "IOCPBuffer.h"
#include "ClientManager.h"
#include "Common/Sington.h"

namespace IOCP
{
	enum E_RECEIVE_PKG_STATUS
	{
		E_RECEIVE_PKG_FINISHED,		// 读包完成，可以先投递新的缓存区，让其他线程继续接收数据包，触发业务回调。
		E_RECEIVE_PKG_UNFINISHED,	// 如果读包，未完成，那么不能回调投递新的数据接收缓存，需要在当前的缓存BUFFER中继续接收。
	};

	typedef std::function<bool(E_RECEIVE_PKG_STATUS, CIOCPBuffer*, std::shared_ptr<CClientContext>&)> ReceivePackageCallBack;

	class IProtocolAnalysisIMPL
	{
	public:
		
		virtual bool NotofySendCompleted(CIOCPBuffer *pIOCPBuffer, std::shared_ptr<CClientContext>& pContext) { return true; };
		
		//************************************
		// Method:    ParseReceivePackage
		// FullName:  IOCP::IProtocolAnalysisIMPL::ParseReceivePackage
		// Access:    virtual public 
		// Returns:   bool 返回失败，中断客户端连接。返回成功，继续接收数据。
		// Qualifier:
		// Parameter: CIOCPBuffer * pIOCPBuffer
		// Parameter: DWORD dwSize
		// Parameter: std::shared_ptr<CClientContext> & pContext
		// Parameter: ReceivePackageCallBack & pCallBack
		//************************************
		virtual bool ParseReceivePackage(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext, ReceivePackageCallBack& pCallBack) = 0;
	};

	class CProtocolAnalysis : public Singleton<CProtocolAnalysis>
	{
	public:

		CProtocolAnalysis();

		enum E_SUPPORT_ANALYSIS
		{
			E_SUPPORT_ANALYSIS_NULL = 0,
			E_SUPPORT_ANALYSIS_COMMON = 1,
			E_SUPPORT_ANALYSIS_WEBSOCKET = 2,
			E_SUPPORT_ANALYSIS_WEBSOCKET_SSL = 3,
			E_MAX_ANALYSIS = 1024,
		};

		bool SetProtocolAnalysisIMPL(E_SUPPORT_ANALYSIS nType, std::shared_ptr<IProtocolAnalysisIMPL> pIMPL);

		bool NotofySendCompleted(int nType, CIOCPBuffer *pIOCPBuffer, std::shared_ptr<CClientContext>& pContext);

		bool ParseReceivePackage(int nType, CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext, ReceivePackageCallBack& pCallBack);

	protected:

		std::shared_ptr<IProtocolAnalysisIMPL> m_IMPL[E_MAX_ANALYSIS];
	};
}

#endif // ProtocolAnalysis_h__
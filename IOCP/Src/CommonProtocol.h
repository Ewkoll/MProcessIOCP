/*****************************************************************************
*  @file     CommonProtocol.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-08 10:03:58
*****************************************************************************/
#ifndef CommonProtocol_h__
#define CommonProtocol_h__

#pragma once
#include <functional>
#include "ProtocolAnalysis.h"
#include "Communication/CommonPack.h"

namespace IOCP
{
	typedef std::function<bool(std::list<CBusinessPacketBody>&, std::shared_ptr<CClientContext>&, IExecutor* pExecutor)> ServerExecute;

	class CCommonProtocol : public IProtocolAnalysisIMPL
	{
	public:

		CCommonProtocol();
		
		virtual ~CCommonProtocol();

		void SetServerExecuteCallBack(ServerExecute pCallBack);

		virtual bool ParseReceivePackage(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext, ReceivePackageCallBack& pCallBack) override;

	protected:
		
		ServerExecute m_pCallBack;	///< 外部服务调用。
		CCommonPack m_CommonPack;	///< 解头部。
	};
}

#endif // CommonProtocol_h__
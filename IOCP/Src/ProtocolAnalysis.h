/*****************************************************************************
*  @file     ProtocolAnalysis.h
*  @brief    Э�����������Э��������������շ����Ƿ�������
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
		E_RECEIVE_PKG_FINISHED,		// ������ɣ�������Ͷ���µĻ��������������̼߳����������ݰ�������ҵ��ص���
		E_RECEIVE_PKG_UNFINISHED,	// ���������δ��ɣ���ô���ܻص�Ͷ���µ����ݽ��ջ��棬��Ҫ�ڵ�ǰ�Ļ���BUFFER�м������ա�
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
		// Returns:   bool ����ʧ�ܣ��жϿͻ������ӡ����سɹ��������������ݡ�
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
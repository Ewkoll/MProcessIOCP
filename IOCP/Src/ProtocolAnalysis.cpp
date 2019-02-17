#include "ProtocolAnalysis.h"
#include "DebugLog.h"
#include "CommonProtocol.h"

IOCP::CProtocolAnalysis::CProtocolAnalysis()
{
	SetProtocolAnalysisIMPL(E_SUPPORT_ANALYSIS_COMMON, std::make_shared<CCommonProtocol>());
}

bool IOCP::CProtocolAnalysis::SetProtocolAnalysisIMPL(E_SUPPORT_ANALYSIS nType, std::shared_ptr<IProtocolAnalysisIMPL> pIMPL)
{
	if (nType < E_SUPPORT_ANALYSIS_NULL || nType >= E_MAX_ANALYSIS)
	{
		return false;
	}

	m_IMPL[nType] = pIMPL;
	return true;
}

bool IOCP::CProtocolAnalysis::NotofySendCompleted(int nType, CIOCPBuffer *pIOCPBuffer, std::shared_ptr<CClientContext>& pContext)
{
	if (nType < E_SUPPORT_ANALYSIS_NULL || nType >= E_MAX_ANALYSIS)
	{
		return false;
	}

	std::shared_ptr<IProtocolAnalysisIMPL> pAnaly = m_IMPL[nType];
	if (NULL == pAnaly)
	{
		DebugFileLog("未实现的协议模式!");
		return false;
	}

	return pAnaly->NotofySendCompleted(pIOCPBuffer, pContext);
}

bool IOCP::CProtocolAnalysis::ParseReceivePackage(int nType, CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext, ReceivePackageCallBack& pCallBack)
{
	if (nType < E_SUPPORT_ANALYSIS_NULL || nType >= E_MAX_ANALYSIS)
	{
		return false;
	}

	std::shared_ptr<IProtocolAnalysisIMPL> pAnaly = m_IMPL[nType];
	if (NULL == pAnaly)
	{
		DebugFileLog("未实现的协议模式!");
		return false;
	}

	return pAnaly->ParseReceivePackage(pIOCPBuffer, dwSize, pContext, pCallBack);
}
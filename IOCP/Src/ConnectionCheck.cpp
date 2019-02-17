#include "ConnectionCheck.h"
#include "CommunicationWorker.h"

CConnectionCheck::CConnectionCheck(bool bBlock, std::string strName /*= std::string()*/)
	: CWorker(bBlock, strName)
	, m_bRunning(false)
{

}

CConnectionCheck::~CConnectionCheck()
{

}

bool CConnectionCheck::Start()
{
	m_bRunning = true;
	return CWorker::Start();
}

bool CConnectionCheck::Run()
{
	int nWaitCount = 0;
	do 
	{
		if (!m_bRunning)
		{
			break;
		}
		
		if (nWaitCount++ < 2 * 30)
		{
			Sleep(500);
			continue;
		}

		nWaitCount = 0;
		IOCP::CTcpClientManager::GetInstance().Vistor(*this);
	} while (true);
	return CWorker::Run();
}

bool CConnectionCheck::Exit()
{
	m_bRunning = false;
	return CWorker::Exit();
}

void CConnectionCheck::Vistor(std::shared_ptr<Node<std::shared_ptr<IOCP::CClientContext>, SOCKET>>& pNode, void *pData /*= NULL*/)
{
	std::shared_ptr<IOCP::CClientContext> pContext = pNode->GetData();
	if (NULL != pContext)
	{
		if (pContext->IsExpired())
		{
			int nConnectTime = pContext->GetConnectTime();
			IOCP::CTcpClientManager::GetInstance().DisconnectClientContext(pContext);
			DebugConsoleLog("连接因为: 长时间没有收发数据被强制中断，地址=【%s】连接持续时间=【%d】", pContext->GetClientAddress().c_str(), nConnectTime);
		}
	}
}
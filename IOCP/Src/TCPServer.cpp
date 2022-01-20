#include "TCPServer.h"
#include "DebugLog.h"
#include "Accept.h"
#include "LockAccept.h"
#include "ErrorCodeMSG.h"
#include "CommunicationWorker.h"

using namespace IOCP;

CTCPServer::CTCPServer(bool bProcessMode /*= false*/, DWORD dwIOWorker /*= CSystemINFO::GetNumberOfProcessors()*/)
	: m_Pool(dwIOWorker)
	, m_pConnectCheck(std::shared_ptr<CConnectionCheck>(new CConnectionCheck(false)))
{
	if (bProcessMode)
	{
		m_pAcceptConnect = std::shared_ptr<CLockAccept>(new CLockAccept());
	}
	else
	{
		m_pAcceptConnect = std::shared_ptr<CAccept>(new CAccept());
	}

	AssociateSocket pCallBack = std::bind(&CTCPServer::AssociateIncomingClientSocket, this, std::placeholders::_1);
	m_pAcceptConnect->SetAssociateSocketCallBack(pCallBack);
}

CTCPServer::~CTCPServer()
{
	Stop();
}

void CTCPServer::SetListenSocket(SOCKET hListenSocket)
{
	m_pAcceptConnect->SetListenSocket(hListenSocket);
}

void CTCPServer::AddBannedIP(SOCKADDR_IN& SockAddr)
{
	m_pAcceptConnect->AddBannedIP(SockAddr);
}

void CTCPServer::RemoveBannedIP(SOCKADDR_IN& SockAddr)
{
	m_pAcceptConnect->RemoveBannedIP(SockAddr);
}

void CTCPServer::SetAcceptConnections(bool bAcceptConnections)
{
	m_pAcceptConnect->SetAcceptConnections(bAcceptConnections);
}

std::string CTCPServer::GetHostIP()
{
	hostent* pHost = gethostbyname("");
	if (NULL == pHost)
	{
		return std::string();
	}
	return std::string(inet_ntoa(*(struct in_addr *)*pHost->h_addr_list));
}

bool CTCPServer::Start()
{
	if (!m_Pool.Start() || !m_pAcceptConnect->Start())
	{
		Stop();
		return false;
	}

	return m_pConnectCheck->Start();
}

bool CTCPServer::Stop()
{
	m_pAcceptConnect->Exit();
	m_pConnectCheck->Exit();
	return m_Pool.Destroy();
}

void CTCPServer::Wait(unsigned long dwMilliseconds)
{
	m_pAcceptConnect->Wait(dwMilliseconds);
}

bool CTCPServer::IsRuning()
{
	return m_pAcceptConnect->IsRuning();
}

void CTCPServer::DisableNagle(SOCKET hClientSocket)
{
	const char chOpt = 1;
	INT nResult = setsockopt(hClientSocket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	if (nResult == SOCKET_ERROR)
	{
		DebugFileLog("��ֹNagle�㷨ʧ��[%p][%s]", hClientSocket, ErrorCode2Text(GetLastError()));
	}
}

unsigned int IOCP::CTCPServer::GetConnectCount()
{
	return CTcpClientManager::GetInstance().GetSize();
}

bool CTCPServer::AssociateIncomingClientSocket(SOCKET& hSocket)
{
	bool bResult = false;
	do 
	{
		if (!CTcpClientManager::GetInstance().IsAccept())
		{
			InfoConsoleLog("CTCPServer-��ǰ�������������ڷ��������������������");
			break;
		}

		if (!m_Pool.Associate((HANDLE)hSocket, hSocket))
		{
			DebugFileLog("CTCPServer-����SOCKET����ɶ˿�ʧ��[%s]", ErrorCode2Text(GetLastError()));
			break;
		}

		if (!CTcpClientManager::GetInstance().AddClient(hSocket, m_Pool.GetCompletionPort()))
		{
			DebugFileLog("CTCPServer-��ӿͻ��˶��󵽹�����ʧ��-�����ϲ����ܵ����߼���");
			break;
		}

		std::shared_ptr<CClientContext> pContext = CTcpClientManager::GetInstance().GetClient(hSocket);
		pContext->SetProtocolType(CProtocolAnalysis::E_SUPPORT_ANALYSIS_COMMON);
		DisableNagle(hSocket);

		CIOCPBuffer *pIOCPBuffer = CTCPBufferManager::GetInstance().AllocateBuffer(OP_IOInitialize);
		if (NULL == pIOCPBuffer)
		{
			CTcpClientManager::GetInstance().RemoveClient(hSocket);
			DebugFileLog("CTCPServer-��ȡBUFFERʧ��-�����ϲ����ܵ����߼���");
			break;
		}

		pIOCPBuffer->SetPoolClient(&CTCPWorker::GetInstance());
		bool bSuccess = (PostQueuedCompletionStatus(m_Pool.GetCompletionPort(), 0, (ULONG_PTR)hSocket, &pIOCPBuffer->m_Overlapped) == TRUE);
		if (!bSuccess)
		{
			INT nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
			{
				DebugFileLog("PostQueuedCompletionStatus Failed[%s]", ErrorCode2Text(nErrorCode).c_str());
				CTcpClientManager::GetInstance().RemoveClient(hSocket);
				CTCPBufferManager::GetInstance().ReleaseBuffer(pIOCPBuffer);
				break;
			}
		}
		bResult = true;
	} while (false);
	return bResult;
}
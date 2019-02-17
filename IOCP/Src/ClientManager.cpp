#include "ClientManager.h"
#include "DebugLog.h"
#include <sstream>
#include "ErrorCodeMSG.h"
using namespace IOCP;

bool CClientManager::AddClient(SOCKET hClientSocket, HANDLE hCompletionPort)
{
	return AddClient(hClientSocket, GetHostAddress(hClientSocket), hCompletionPort);
}

bool CClientManager::AddClient(SOCKET hClientSocket, const std::string& strClientAddress, HANDLE hCompletionPort)
{
	std::shared_ptr<CClientContext> pContext = GetClient(hClientSocket);
	bool bResult = false;
	do 
	{
		if (NULL != pContext)
		{
			DebugFileLog("添加一个已经存在(%d)的SOCKET到客户端管理器！", hClientSocket);
			break;
		}

		pContext = std::make_shared<CClientContext>(hClientSocket, strClientAddress);
		pContext->SetCompletionPort(hCompletionPort);
		bResult = m_mapClientContext.AddNode(hClientSocket, pContext);
		if (!bResult)
		{
			break;
		}

		if (NULL != m_pCallBack)
		{
			m_pCallBack(pContext, true);
		}

		DebugConsoleLog("添加（%d）SOCKET成功！客户端地址（%s）", hClientSocket, pContext->GetClientAddress().c_str());
	} while (false);
	return bResult;
}

bool CClientManager::RemoveClient(SOCKET hClientSocket)
{
	std::shared_ptr<CClientContext> pContext = GetClient(hClientSocket);
	if (NULL == pContext)
	{
		return false;
	}
	
	if (m_mapClientContext.RemoveNode(hClientSocket))
	{
		DisConnectClient(hClientSocket);
		if (NULL != m_pCallBack)
		{
			m_pCallBack(pContext, false);
		}

		DebugConsoleLog("断开（%d）SOCKET成功！客户端地址（%s）", hClientSocket, pContext->GetClientAddress().c_str());
	}
	return true;
}

bool CClientManager::DisconnectClientContext(std::shared_ptr<CClientContext>& pContext)
{
	DisConnectClient(pContext->GetSocket());
	return true;
}

void CClientManager::UpdateClientTime(SOCKET hClientSocket)
{
	std::shared_ptr<CClientContext> pContext = GetClient(hClientSocket);
	if (NULL != pContext)
	{
		pContext->UpdateTime();
	}
}

void CClientManager::Vistor(IVistor<std::shared_ptr<CClientContext>, SOCKET>& Vist)
{
	m_mapClientContext.VistorNode(Vist);
}

void CClientManager::SetNotifyCallBack(NotifyCallBack pCallBack)
{
	m_pCallBack = pCallBack;
}

std::shared_ptr<CClientContext> CClientManager::GetClient(SOCKET hClientSocket)
{
	std::shared_ptr<Node<std::shared_ptr<CClientContext>, SOCKET>> pNode = m_mapClientContext[hClientSocket];
	if (NULL == pNode)
	{
		return std::shared_ptr<CClientContext>();
	}
	return pNode->GetData();
}

size_t CClientManager::GetSize()
{
	return m_mapClientContext.GetSize();
}

bool CClientManager::DisConnectClient(SOCKET hSocket)
{
	if (hSocket == INVALID_SOCKET)
	{
		return false;
	}

	LINGER LingerStruct = LINGER{ 1, 0 };
	setsockopt(hSocket, SOL_SOCKET, SO_LINGER, (char *)&LingerStruct, sizeof(LingerStruct));

	CancelIo((HANDLE)hSocket);

	closesocket(hSocket);
	hSocket = INVALID_SOCKET;
	return true;
}

std::string CClientManager::GetHostAddress(SOCKET hClientSocket)
{
	SOCKADDR_IN SockIN = { 0 };
	size_t nSockAddrLen = sizeof(SOCKADDR_IN);
	INT nResult = getpeername(hClientSocket, (SOCKADDR*)&SockIN, &(INT&)nSockAddrLen);
	if (nResult == INVALID_SOCKET)
	{
		return std::string();
	}

	std::stringstream StringStream;
	StringStream << inet_ntoa(SockIN.sin_addr) << ":" << SockIN.sin_port;
	return StringStream.str();
}
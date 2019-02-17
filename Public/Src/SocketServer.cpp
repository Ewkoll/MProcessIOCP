#include "SocketServer.h"

CSocketServer::CSocketServer()
	: m_hSocket(INVALID_SOCKET)
{

}

CSocketServer::~CSocketServer()
{
	StopListen();
}

SOCKET CSocketServer::GetServerSocket()
{
	return m_hSocket;
}

void CSocketServer::StopListen()
{
	if (INVALID_SOCKET == m_hSocket)
	{
		return;
	}
	
	closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;
}

SOCKET CSocketServer::Listen(const std::string& strAddress, int nPort)
{
	SOCKET hSocket = INVALID_SOCKET;
	bool bResult = false;
	do 
	{
		hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (hSocket == INVALID_SOCKET)
		{
			break;
		}

		BOOL bReUse = TRUE;
		INT nResult = setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReUse, sizeof(BOOL));
		if (nResult < 0)
		{
			break;
		}

		SOCKADDR_IN ListenAddr = { 0 };
		ListenAddr.sin_family = AF_INET;
		ListenAddr.sin_port = htons(nPort);
		if (strAddress.empty())
		{
			ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			inet_pton(AF_INET, strAddress.c_str(), (void*)&ListenAddr.sin_addr);
		}

		nResult = bind(hSocket, (PSOCKADDR)&ListenAddr, sizeof(ListenAddr));
		if (nResult == SOCKET_ERROR)
		{
			break;
		}
		
		nResult = listen(hSocket, SOMAXCONN);
		if (nResult == SOCKET_ERROR)
		{
			break;
		}

		bResult = true;
	} while (false);
	
	bResult ? m_hSocket = hSocket : StopListen();
	return m_hSocket;
}
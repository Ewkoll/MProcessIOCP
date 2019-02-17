#include "SocketLayer.h"
#include <sstream>
#include "ErrorCodeMSG.h"

#define CHECK_SOCKET(hSocket) if (INVALID_SOCKET == hSocket){ return false; };
#define CHECK_RESULT(nResult) if (SOCKET_ERROR == nResult){ OutputDebugStringA(ErrorCode2Text(WSAGetLastError()).c_str()); };

CInitSocket CSocketLayer::m_InitSocket;

CSocketLayer::CSocketLayer(SOCKET hSocket /*= INVALID_SOCKET*/)
	: m_hSocket(INVALID_SOCKET)
{

}

bool CSocketLayer::CloseSocket()
{
	CHECK_SOCKET(m_hSocket);
	SetLinger(true, 0);
	CancelIo((HANDLE)m_hSocket);
	closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;
	return true;
}

bool CSocketLayer::ShutDownSocket(int nSD /*= SD_BOTH*/)
{
	CHECK_SOCKET(m_hSocket);
	INT nResult = shutdown(m_hSocket, nSD);
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::GetPeerName(SOCKADDR* pAddress, int* pLength)
{
	CHECK_SOCKET(m_hSocket);
	INT nResult = getpeername(m_hSocket, pAddress, pLength);
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

std::string CSocketLayer::GetPeerName()
{
	SOCKADDR_IN SockIN = { 0 };
	int nSockAddrLen = sizeof(SOCKADDR_IN);
	if (GetPeerName((SOCKADDR*)&SockIN, &nSockAddrLen))
	{
		std::stringstream StringStream;
		StringStream << inet_ntoa(SockIN.sin_addr) << ":" << SockIN.sin_port;
		return StringStream.str();
	}
	return std::string();
}

bool CSocketLayer::SetBlock(bool bBlock)
{
	CHECK_SOCKET(m_hSocket)
	int nBlock = 0;
	bBlock ? nBlock = 1 : nBlock = 0;
	INT nResult = ioctlsocket(m_hSocket, FIONBIO, (unsigned long *)&nBlock);
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::Nagle(bool bEnable)
{
	CHECK_SOCKET(m_hSocket)
	char chOpt = 1;
	bEnable ? chOpt = 0 : chOpt = 1;
	INT nResult = setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetSndBuffer(int nSize)
{
	CHECK_SOCKET(m_hSocket)
	INT nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&nSize, sizeof(nSize));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetRcvBuffer(int nSize)
{
	CHECK_SOCKET(m_hSocket)
	INT nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&nSize, sizeof(nSize));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetSndTime(int nSecond, int Microseconds)
{
	CHECK_SOCKET(m_hSocket)
	struct timeval TimeVal = { nSecond, Microseconds };
	INT nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&TimeVal, sizeof(TimeVal));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetRcvTime(int nSecond, int Microseconds)
{
	CHECK_SOCKET(m_hSocket)
	struct timeval TimeVal = {nSecond, Microseconds};
	INT nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TimeVal, sizeof(TimeVal));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetLinger(bool bStatus, int nLingerTime)
{
	CHECK_SOCKET(m_hSocket)
	LINGER LingerStruct = { 0 };
	LingerStruct.l_onoff = bStatus ? 1 : 0;
	LingerStruct.l_linger = nLingerTime;
	INT nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (char *)&LingerStruct, sizeof(LingerStruct));
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::SetSockOpt(int nOptionName, int nLevel, const void* pOptionValue, int nOptionLength)
{
	CHECK_SOCKET(m_hSocket)
	INT nResult = setsockopt(m_hSocket, nOptionName, nLevel, (LPCSTR)pOptionValue, nOptionLength);
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}

bool CSocketLayer::GetSockOpt(int nOptionName, int nLevel, void* pOptionValue, int* nOptionLength)
{
	CHECK_SOCKET(m_hSocket)
	INT nResult = getsockopt(m_hSocket, nOptionName, nLevel, (LPSTR)pOptionValue, nOptionLength);
	CHECK_RESULT(nResult)
	return nResult != SOCKET_ERROR;
}
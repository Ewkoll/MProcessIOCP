#include "InitSocket.h"

#pragma comment(lib, "ws2_32.lib")

CInitSocket::CInitSocket() : m_bInitResult(false)
{
	WORD wVersionRequest = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };

	INT nResult = WSAStartup(wVersionRequest, &wsaData);
	if (0 != nResult)
	{
		m_strErrorMSG = "WSAStartup Failed!";
	}
	else
	{
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			m_strErrorMSG = "WSAStartup Version(2, 2) Failed!";
			WSACleanup();
		}
		else
		{
			m_bInitResult = true;
		}
	}
}

CInitSocket::~CInitSocket()
{
	if (m_bInitResult)
	{
		WSACleanup();
	}
}

bool CInitSocket::GetInitResult(std::string& strErrorMSG)
{
	strErrorMSG = m_strErrorMSG;
	return m_bInitResult;
}
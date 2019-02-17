/*****************************************************************************
*  @file     SocketLayer.h
*  @brief    套接字层。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-11 15:33:22
*****************************************************************************/
#ifndef SocketLayer_h__
#define SocketLayer_h__

#pragma once
#include "InitSocket.h"

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif

enum E_SOCKET_ERROR_TYPE
{
	SOCKET_ERROR_STOP = -1,
	SOCKET_ERROR_INVALID_SOCKET = -2,
	SOCKET_ERROR_SELECT = -3,
	SOCKET_ERROR_SEND_TIMEOUT = -4,
	SOCKET_ERROR_RECV_TIMEOUT = -5,
	SOCKET_ERROR_SEND_EXCEPTION = -6,
	SOCKET_ERROR_RECV_EXCEPTION = -7,
	SOCKET_ERROR_SEND = -8,
	SOCKET_ERROR_RECV = -9,
	SOCKET_ERROR_CLOSED = -10,
};

class CSocketLayer
{
public:

	CSocketLayer(SOCKET hSocket = INVALID_SOCKET);

	SOCKET GetSocket() { return m_hSocket; };

	void SetSocket(SOCKET hSocket) { m_hSocket = hSocket; };
	
	bool CloseSocket();

	bool ShutDownSocket(int nSD = SD_BOTH);

	std::string GetPeerName();

	bool GetPeerName(SOCKADDR* pAddress, int* pLength);

	bool SetBlock(bool bBlock);

	bool Nagle(bool bEnable);

	bool SetSndBuffer(int nSize);

	bool SetRcvBuffer(int nSize);

	bool SetSndTime(int nSecond, int Microseconds);

	bool SetRcvTime(int nSecond, int Microseconds);

	bool SetLinger(bool bStatus, int nLingerTime);
	
	bool SetSockOpt(int nOptionName, int nLevel, const void* pOptionValue, int nOptionLength);
	
	bool GetSockOpt(int nOptionName, int nLevel, void* pOptionValue, int* nOptionLength);
	
protected:
	SOCKET m_hSocket;					///< 套接字。
	static CInitSocket m_InitSocket;	///< 网络库初始化。
};

#endif // SocketLayer_h__
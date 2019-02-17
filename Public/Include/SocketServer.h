/*****************************************************************************
*  @file     SocketServer.h
*  @brief    套接字服务器基类
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/06/08 09:20:18
*****************************************************************************/
#ifndef SocketServer_h__
#define SocketServer_h__

#pragma once
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

class CSocketServer
{
public:

	CSocketServer();

	virtual ~CSocketServer();

	SOCKET GetServerSocket();

	void StopListen();

	virtual SOCKET Listen(const std::string& strAddress, int nPort);

private:

	SOCKET m_hSocket;	///< 监听套接字。
};

#endif // SocketServer_h__
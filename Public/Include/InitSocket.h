/*****************************************************************************
*  @file     InitSocket.h
*  @brief    WSAStartup 调用次数，匹配WSACleanup。禁止在DLLMain中调用。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/03/14 14:13:09
*****************************************************************************/
#ifndef InitSocket_h__
#define InitSocket_h__

#pragma once
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

class CInitSocket
{
public:
	
	CInitSocket();

	virtual ~CInitSocket();

	bool IsInitSuccess() { return m_bInitResult; };

	bool GetInitResult(std::string& strErrorMSG);

private:
	bool m_bInitResult;			///< 初始化结果。
	std::string m_strErrorMSG;	///< 错误消息。
};

#endif // InitSocket_h__
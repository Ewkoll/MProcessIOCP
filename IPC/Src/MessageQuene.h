/*****************************************************************************
*  @file     MessageQuene.h
*  @brief    包装Boost的消息队列，需要管理员权限运行程序。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-28 14:28:39
*****************************************************************************/
#ifndef MessageQuene_h__
#define MessageQuene_h__
#pragma once
#ifndef _SCL_SECURE_NO_WARNINGS
#	define _SCL_SECURE_NO_WARNINGS
#endif // !_SCL_SECURE_NO_WARNINGS
#include "IPC.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

class CMessageQuene : public IPC
{
public:

	CMessageQuene() {};

	virtual ~CMessageQuene() { };
		
	virtual bool InitIPC(const char* pIpcName, E_IPC_MODE nMode, size_t nMaxMsg, size_t nMaxMsgSize) override;
	
	virtual bool UnInitIPC(const char* pIpcName) override;

	virtual bool Send(void* pBuffer, size_t nLength) override;
	
	virtual bool TimeSend(void* pBuffer, size_t nLength, size_t nMillisecond) override;
	
	virtual bool Receive(void* pBuffer, size_t nLength) override;
	
	virtual bool TimeReceive(void* pBuffer, size_t nLength, size_t nMillisecond) override;

private:
	boost::shared_ptr<boost::interprocess::message_queue> m_pMessageQueue;
};

#endif // MessageQuene_h__
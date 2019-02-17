/*****************************************************************************
*  @file     Accept.h
*  @brief    接受连接线程模式。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/06/07 19:17:39
*****************************************************************************/
#ifndef Accept_h__
#define Accept_h__

#pragma once
#include <map>
#include "AcceptBase.h"

class CAccept : public CAcceptBase
{
public:

	CAccept(std::string strName = std::string());

	virtual ~CAccept();
		
	virtual bool Start() override;

	virtual bool Run() override;

	virtual bool Exit() override;

protected:
	
	void StopWaitEvent()
	{
		if (m_hWSAEvent != WSA_INVALID_EVENT)
		{
			WSACloseEvent(m_hWSAEvent);
			m_hWSAEvent = WSA_INVALID_EVENT;
		}
	}

private:
	HANDLE m_hWSAEvent;										///< 网络事件。
	volatile bool m_bRunning;								///< 运行状态。
};

#endif // Accept_h__
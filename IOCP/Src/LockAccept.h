/*****************************************************************************
*  @file     LockAccept.h
*  @brief    进程模式下接受连接。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                 
*  @email    IDeath@operatorworld.com                             
*  @version  0.0.0.1														
*  @date	 19/01/28 16:13:58
*****************************************************************************/
#ifndef LockAccept_h__
#define LockAccept_h__

#pragma once
#include "AcceptBase.h"

class CLockAccept : public CAcceptBase
{
public:
	CLockAccept(std::string strName = std::string());

	virtual ~CLockAccept();

	virtual bool Start() override;

	virtual bool Run() override;

	virtual bool Exit() override;

private:

	volatile bool m_bRunning;								///< 运行状态。
};
#endif // LockAccept_h__
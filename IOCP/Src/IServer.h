/*****************************************************************************
*  @file     IServer.h
*  @brief    服务器类型抽象接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-03 09:34:51
*****************************************************************************/
#ifndef IServer_h__
#define IServer_h__
#pragma once

class IServer
{
public:
	virtual bool Start() = 0;

	virtual bool Stop() = 0;

	virtual bool IsRuning() = 0;

	virtual void Wait(unsigned long dwMilliseconds) = 0;
};

#endif // IServer_h__
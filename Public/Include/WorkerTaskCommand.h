/*****************************************************************************
*  @file     WorkerTaskCommand.h
*  @brief    工作任务系统。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-31 10:49:50
*****************************************************************************/
#ifndef WorkerTaskCommand_h__
#define WorkerTaskCommand_h__

#pragma once
#include "IPCParam.h"
#include "TCPServer.h"

class CWorkerTaskCommand
{
public:
	
	CWorkerTaskCommand(IPCParam& IPCParam);

	~CWorkerTaskCommand();
	
	bool Run();

	bool CheckMasterExist();

private:
	
	std::shared_ptr<IOCP::CTCPServer> GetServer();

private:
	IPCParam m_IPCParam;
	std::shared_ptr<IOCP::CTCPServer> m_pIOCPServer;
};

#endif // WorkerTaskCommand_h__

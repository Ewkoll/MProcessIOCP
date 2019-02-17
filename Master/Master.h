/*****************************************************************************
*  @file     Master.h
*  @brief    主进程。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-27 10:19:33
*****************************************************************************/
#pragma once
#include <list>
#include "InitSocket.h"
#include "SocketServer.h"
#include "MasterIPC.h"

class CWorkerProcessData
{
public:
	CWorkerProcessData(DWORD nServerID, DWORD dwClientID, PROCESS_INFORMATION& ProcessINFO)
		: m_ProcessINFO(ProcessINFO)
		, m_IPC(nServerID, dwClientID)
	{

	}

	virtual ~CWorkerProcessData() { m_IPC.UnInit(); }

	DWORD GetWorkerPID() { return m_ProcessINFO.dwProcessId; }

	CMasterIPC& GetIPC() { return m_IPC; }

	PROCESS_INFORMATION& GetINFO() { return m_ProcessINFO; }

private:
	CMasterIPC m_IPC;
	PROCESS_INFORMATION m_ProcessINFO;
};

class CMaster : public CSocketServer
{
public:
	
	CMaster(std::string strPath, int nCount);

	virtual ~CMaster();
	
	bool Init(std::string strAddress, int nPort);

	void Run();

	bool UnInit();
	
private:
	std::list<std::shared_ptr<CWorkerProcessData>> m_WorkerProcess;
	std::string m_strPath;
	int m_nCount;
	CInitSocket m_InitSocket;
};
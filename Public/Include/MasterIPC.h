/*****************************************************************************
*  @file     MasterIPC.h
*  @brief    不支持多线程。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-28 16:00:01
*****************************************************************************/
#ifndef MasterIPC_h__
#define MasterIPC_h__

#pragma once
#include "IPC.h"
#include "IPCHeader.h"

class CMasterIPC
{
public:

	CMasterIPC(DWORD dwServerID, DWORD dwWorkerID);

	~CMasterIPC();
	
	bool Init();

	bool GetIPCStatus();

	void ReInit();

	bool WaitInitFinish();

	bool UnInit();
	
	bool Ping();

	bool Pong(DWORD dwTimeOut = IPC_TIMEOUT);

	bool SendExitCommand();
	
	bool SendData(E_IPC_PACKET_TYPE nType);

	bool TimeSendData(E_IPC_PACKET_TYPE nType, DWORD dwTimeOut = IPC_TIMEOUT);

	bool ReceiveData(CIPCData& IPCData);

	bool TimeReceiveData(CIPCData& IPCData, DWORD dwTimeOut = IPC_TIMEOUT);

	template<typename T> 
	bool SendData(E_IPC_PACKET_TYPE nType, T& tData)
	{
		CIPCData IPCData;
		IPCData.SetData<T>(nType, tData);
		return m_IPCServer.Send(&IPCData, sizeof(CIPCData));
	}

private:
	IPCModule<CIPCData> m_IPCServer;		///< 发送数据队列。
	IPCModule<CIPCData> m_IPCClient;		///< 接收数据队列。
};

#endif // MasterIPC_h__
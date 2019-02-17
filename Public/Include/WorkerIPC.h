/*****************************************************************************
*  @file     WorkerIPC.h
*  @brief    工作进程IPC。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-31 09:45:38
*****************************************************************************/
#ifndef WorkerIPC_h__
#define WorkerIPC_h__

#pragma once
#include "IPC.h"
#include "IPCHeader.h"

class CWorkerIPC
{
public:
	
	CWorkerIPC(DWORD dwServerID, DWORD dwWorkerID);

	~CWorkerIPC();

	bool Init();

	bool GetIPCStatus();

	void ReInit();
	
	bool WaitInitFinish();

	bool UnInit();

	bool Ping();

	bool Pong();
	
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
	IPCModule<CIPCData> m_IPCServer;		///< 发送数据队列-对应MasterIPC的Client。
	IPCModule<CIPCData> m_IPCClient;		///< 接收数据队列-对应MasterIPC的Server。
};

#endif // WorkerIPC_h__
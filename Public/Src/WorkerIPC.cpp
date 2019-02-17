#include "WorkerIPC.h"
#include "DebugLog.h"

CWorkerIPC::CWorkerIPC(DWORD dwServerID, DWORD dwWorkerID)
	: m_IPCServer(dwServerID, dwWorkerID, WORKER_IPCNAME, true, E_IPC_MODE_SENDER)
	, m_IPCClient(dwServerID, dwWorkerID, MASTER_IPCNAME, false, E_IPC_MODE_RECEIVER)
{
	
}

CWorkerIPC::~CWorkerIPC()
{
	UnInit();
}

bool CWorkerIPC::Init()
{
	if (!Ping() || !Pong())
	{
		return false;
	}
	return true;
}

bool CWorkerIPC::GetIPCStatus()
{
	return m_IPCServer.GetStatus() && m_IPCClient.GetStatus();
}

void CWorkerIPC::ReInit()
{
	if (!m_IPCServer.GetStatus())
	{
		m_IPCServer.ReInitIPC();
	}

	if (!m_IPCClient.GetStatus())
	{
		m_IPCClient.ReInitIPC();
	}
}

bool CWorkerIPC::WaitInitFinish()
{
	int nWaitCount = 0;
	do
	{
		if (!GetIPCStatus())
		{
			Sleep(50);
			ReInit();
		}
		else
		{
			break;
		}
	} while (nWaitCount++ < 1024);
	return Init();
}

bool CWorkerIPC::UnInit()
{
	m_IPCServer.FreeIPCModule();
	m_IPCClient.FreeIPCModule();
	return true;
}

bool CWorkerIPC::Ping()
{
	CIPCData IPCData;
	if (ReceiveData(IPCData) && IPCData.m_nPacketType == E_IPC_PACKET_TYPE_PING)
	{
		return true;
	}

	DebugConsoleLog("收到PING包错误,TYPE=[%d]", IPCData.m_nPacketType);
	return false;
}

bool CWorkerIPC::Pong()
{
	return SendData(E_IPC_PACKET_TYPE_PONG);
}

bool CWorkerIPC::SendData(E_IPC_PACKET_TYPE nType)
{
	return m_IPCServer.Send(&CIPCData(nType), sizeof(CIPCData));
}

bool CWorkerIPC::TimeSendData(E_IPC_PACKET_TYPE nType, DWORD dwTimeOut /*= IPC_TIMEOUT*/)
{
	return m_IPCServer.TimeSend(&CIPCData(nType), sizeof(CIPCData), dwTimeOut);
}

bool CWorkerIPC::ReceiveData(CIPCData& IPCData)
{
	return m_IPCClient.Receive(&IPCData, sizeof(IPCData));
}

bool CWorkerIPC::TimeReceiveData(CIPCData& IPCData, DWORD dwTimeOut /*= IPC_TIMEOUT*/)
{
	return m_IPCClient.TimeReceive(&IPCData, sizeof(IPCData), dwTimeOut);
}
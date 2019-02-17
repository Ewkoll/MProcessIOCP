#include "MasterIPC.h"
#include "DebugLog.h"

CMasterIPC::CMasterIPC(DWORD dwServerID, DWORD dwWorkerID)
	: m_IPCServer(dwServerID, dwWorkerID, MASTER_IPCNAME, true, E_IPC_MODE_SENDER)
	, m_IPCClient(dwServerID, dwWorkerID, WORKER_IPCNAME, false, E_IPC_MODE_RECEIVER)
{

}

CMasterIPC::~CMasterIPC()
{
	UnInit();
}

bool CMasterIPC::Init()
{
	if (!Ping() || !Pong())
	{
		return false;
	}
	return true;
}

bool CMasterIPC::GetIPCStatus()
{
	return m_IPCServer.GetStatus() && m_IPCClient.GetStatus();
}

void CMasterIPC::ReInit()
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

bool CMasterIPC::WaitInitFinish()
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

bool CMasterIPC::UnInit()
{
	m_IPCServer.FreeIPCModule();
	m_IPCClient.FreeIPCModule();
	return true;
}

bool CMasterIPC::SendExitCommand()
{
	bool bResult = false;
	do 
	{
		if (!SendData(E_IPC_PACKET_TYPE_EXIT_COMMAND))
		{
			break;
		}

		CIPCData IPCData;
		if (ReceiveData(IPCData) && IPCData.m_nPacketType == E_IPC_PACKET_TYPE_EXIT_RESULT)
		{
			CResult &Result = IPCData.GetData<CResult>();
			bResult = (Result.m_nCode == E_IPC_CODE_EXIT_SUCCESS);
		}
	} while (false);
	return bResult;
}

bool CMasterIPC::Ping()
{
	return SendData(E_IPC_PACKET_TYPE_PING);
}

bool CMasterIPC::Pong(DWORD dwTimeOut /* = IPC_TIMEOUT */)
{
	CIPCData IPCData;
	if (ReceiveData(IPCData) && IPCData.m_nPacketType == E_IPC_PACKET_TYPE_PONG)
	{
		return true;	
	}

	DebugConsoleLog("收到Pong包错误,TYPE=[%d]", IPCData.m_nPacketType);
	return false;
}

bool CMasterIPC::SendData(E_IPC_PACKET_TYPE nType)
{
	return m_IPCServer.Send(&CIPCData(nType), sizeof(CIPCData));
}

bool CMasterIPC::TimeSendData(E_IPC_PACKET_TYPE nType, DWORD dwTimeOut /*= IPC_TIMEOUT*/)
{
	return m_IPCServer.TimeSend(&CIPCData(nType), sizeof(CIPCData), dwTimeOut);
}

bool CMasterIPC::ReceiveData(CIPCData& IPCData)
{
	return m_IPCClient.Receive(&IPCData, sizeof(IPCData));
}

bool CMasterIPC::TimeReceiveData(CIPCData& IPCData, DWORD dwTimeOut /*= IPC_TIMEOUT*/)
{
	return m_IPCClient.TimeReceive(&IPCData, sizeof(IPCData), dwTimeOut);
}
#include <sstream>
#include "WorkerTaskCommand.h"
#include "WorkerIPC.h"
#include "DebugLog.h"
#include "ProcessM.h"
#include <WinSock2.h>

CWorkerTaskCommand::CWorkerTaskCommand(IPCParam& IPCParam)
	: m_IPCParam(IPCParam)
{

}

CWorkerTaskCommand::~CWorkerTaskCommand()
{

}

std::shared_ptr<IOCP::CTCPServer> CWorkerTaskCommand::GetServer()
{
	if (NULL == m_pIOCPServer)
	{
		m_pIOCPServer = std::make_shared<IOCP::CTCPServer>(true);
	}
	return m_pIOCPServer;
}

bool CWorkerTaskCommand::Run()
{
	DWORD dwWorkerID = GetCurrentProcessId();
	DebugConsoleLog("CWorkerTaskCommand MasterID [%d] WorkerID=[%d]", m_IPCParam.GetMasterID(), dwWorkerID);
	CWorkerIPC WorkerIPC(m_IPCParam.GetMasterID(), dwWorkerID);
	if (!WorkerIPC.WaitInitFinish())
	{
		DebugConsoleLog("CWorkerTaskCommand Init Failed");
		return false;
	}
	
	bool bRuning = true;
	do
	{
		CIPCData IPCData;
		if (!WorkerIPC.TimeReceiveData(IPCData))
		{
			if (!CheckMasterExist())
			{
				break;
			}
			else
			{
				continue;
			}
		}

		std::shared_ptr<IOCP::CTCPServer> pServer = GetServer();
		switch (IPCData.m_nPacketType)
		{
		case E_IPC_PACKET_TYPE_PING:
			{
				DebugConsoleLog("E_IPC_PACKET_TYPE_PING");
				InfoConsoleLog("当前连接数：%d", pServer->GetConnectCount());
				WorkerIPC.Pong();
			}
			break;
		case E_IPC_PACKET_TYPE_EXIT_COMMAND:
			{
				DebugConsoleLog("E_IPC_PACKET_TYPE_EXIT_COMMAND");
				pServer->Stop();

				std::stringstream StringStram;
				StringStram << "ChildProcess " << GetCurrentProcessId() << " Exiting!";

				CIPCData IPCData;
				IPCData.SetData(E_IPC_PACKET_TYPE_EXIT_RESULT, CResult(E_IPC_CODE_EXIT_SUCCESS, StringStram.str()));
				WorkerIPC.SendData(E_IPC_PACKET_TYPE_EXIT_RESULT, IPCData);
				bRuning = false;
			}
			break;
		case E_IPC_PACKET_TYPE_WSASOCKET_COMMAND:
			{
				DebugConsoleLog("E_IPC_PACKET_TYPE_WSASOCKET_COMMAND");
				bool bIsRunning = pServer->IsRuning();
				WSAPROTOCOL_INFO& WsaINFO = IPCData.GetData<WSAPROTOCOL_INFO>();
				SOCKET hListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, &WsaINFO, 0, WSA_FLAG_OVERLAPPED);
				pServer->SetListenSocket(hListenSocket);
				pServer->Start();
			}
			break;
		}
	} while (bRuning);
	return true;
}

bool CWorkerTaskCommand::CheckMasterExist()
{
	CProcessM ProcessM;
	return ProcessM.IsProcessRunning(m_IPCParam.GetMasterName(), m_IPCParam.GetMasterID());
}
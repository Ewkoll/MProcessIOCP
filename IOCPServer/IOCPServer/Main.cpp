#include "stdafx.h"
#include "WorkerIPC.h"
#include "WorkerTaskCommand.h"
#include "TCPServer.h"
#include "ProcessM.h"
#include "Communication/IOPack.h"
#include "Communication/CommonPack.h"

void RunServer()
{
	WORD wVersionRequest = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };
	WSAStartup(wVersionRequest, &wsaData);

	std::shared_ptr<IOCP::CTCPServer> pServer(new IOCP::CTCPServer());
	SOCKET hListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN ListenAddr = { 0 };
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_port = htons(8055);
	ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(hListenSocket, (PSOCKADDR)&ListenAddr, sizeof(ListenAddr));
	listen(hListenSocket, SOMAXCONN);

	pServer->SetListenSocket(hListenSocket);
	pServer->Start();
	pServer->Wait(INFINITE);
	pServer->Stop();
	WSACleanup();
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		RunServer();
		return 0;
	}
	
	std::string strMasterID = argv[1];
	DWORD dwMasterID = (DWORD)atoi(strMasterID.c_str());
	DebugConsoleLog("Master PID : %d", dwMasterID);
	if (0 == dwMasterID)
	{
		return 0;
	}

	std::string strMasterName = argv[2];
	CProcessM ProcessM;
	std::string strPIDMasterName = ProcessM.GetNameByPid(dwMasterID);
	DebugConsoleLog("Master Name : %s", strMasterName.c_str());
	if (strPIDMasterName != strMasterName)
	{
		return 0;
	}

	IPCParam Param;
	Param.SetMasterID(dwMasterID);
	Param.SetMasterName(strMasterName);
	CWorkerTaskCommand TaskCommand(Param);
	if (!TaskCommand.Run()){ DebugConsoleLog("TaskCommand Run Failed..."); }
	return 0;
}
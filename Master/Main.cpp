#include "stdafx.h"
#include <iostream>
#include "IPCHeader.h"
#include "Master.h"
#include <thread>
#include "ProcessM.h"
#include "WorkPath.h"

//#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")

int main(int argc, char* argv[])
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, "LockAccept");
	CWorkPath WorkPath;
	std::string strModulePath = WorkPath.GetModulePath();
	WorkPath.SetWorkPath(strModulePath);
	CMaster Master("IOCPServer.exe", 4);
	Master.Init(std::string(), 8055);
	Master.Run();
	Master.UnInit();
	CloseHandle(hMutex);
	return 0;
}
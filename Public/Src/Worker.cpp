#include "Worker.h"
#include <Windows.h>

CWorker::CWorker(bool bBlock, std::string strName /*= std::string()*/)
	: m_ExitEvent(true, true, strName)
{
	m_RunThread.SetName(strName);
	m_bBlock = bBlock;
}

CWorker::~CWorker()
{
	
}

bool CWorker::Init()
{
	return true;
}

bool CWorker::Start()
{
	m_RunThread.Attach(shared_from_this());
	m_ExitEvent.ResetEvent();
	return m_RunThread.Run(m_bBlock);
}

bool CWorker::Run()
{
	m_ExitEvent.SetEvent();
	return true;
}

bool CWorker::Exit()
{
	if (!IsRuning())
	{
		return true;
	}
	
	DWORD dwResult = m_ExitEvent.WaitForSingleObject(INFINITE);
	if (dwResult != WAIT_OBJECT_0)
	{
		return m_RunThread.TerminateThread();
	}
	return true;
}

bool CWorker::IsRuning()
{
	DWORD dwResult = m_ExitEvent.WaitForSingleObject(0);
	return dwResult != WAIT_OBJECT_0;
}

void CWorker::Wait(unsigned long dwMilliseconds)
{
	m_ExitEvent.WaitForSingleObject(dwMilliseconds);
}
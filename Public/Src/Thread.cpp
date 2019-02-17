#include "Thread.h"
#include <Windows.h>
#include <functional>

CThread::CThread()
{
	m_bRunStatus = false;
}

CThread::~CThread()
{
	m_bRunStatus = false;
}

void CThread::Attach(const std::shared_ptr<IRunnable>& pRunable)
{
	m_pRunable = pRunable;
}

void CThread::SetName(const std::string& strName)
{
	m_strName = strName;
}

std::string CThread::GetName() const
{
	return m_strName;
}

bool CThread::Run(bool bBlock)
{
	bool bResult = false;
	do 
	{
		if (m_bRunStatus)
		{
			break;
		}

		if (m_pRunable.expired())
		{
			break;
		}

		std::shared_ptr<IRunnable> pRunable = m_pRunable.lock();
		if (nullptr == pRunable)
		{
			break;
		}
		
		if (!bBlock)
		{
			//非Block运行线程的时候，线程不能被中断，后续的暂停线程，相关的函数全部失效。
			//会增加pRunable的引用计数数量，线程不结束不会释放。
			m_RunThread.swap(std::thread(std::bind(&IRunnable::Run, pRunable)));
			m_RunThread.detach();
			m_bRunStatus = true;
			bResult = true;
		}
		else
		{
			//Block运行线程，等待线程执行结束。可能会被中断，不能增加引用计数，导致内存无法释放。
			m_RunThread.swap(std::thread(std::bind(&IRunnable::Run, std::ref(pRunable))));
			m_bRunStatus = true;
			try
			{
				m_RunThread.join();
				bResult = true;
			}
			catch (std::exception& Exception)
			{
				std::string strEWhat = Exception.what();
				strEWhat.empty();
			}
		}
	} while (false);
	return bResult;
}

bool CThread::GetThreadPriority(int& nPriority)
{
	if (!m_RunThread.joinable())
	{
		return false;
	}

	nPriority = ::GetThreadPriority(m_RunThread.native_handle());
	return true;
}

bool CThread::SetThreadPriority(int nPriority)
{
	if (!m_RunThread.joinable())
	{
		return false;
	}

	return ::SetThreadPriority(m_RunThread.native_handle(), nPriority) == TRUE;
}

bool CThread::SuspendThread()
{
	if (!m_RunThread.joinable())
	{
		return false;
	}
	
	return ::SuspendThread(m_RunThread.native_handle()) > 0;
}

bool CThread::ResumeThread()
{
	if (!m_RunThread.joinable())
	{
		return false;
	}

	do {} while (::ResumeThread(m_RunThread.native_handle()) > 0);
	return true;
}

bool CThread::TerminateThread(int nExitCode /* = 0 */)
{
	if (!m_RunThread.joinable())
	{
		return false;
	}

	bool bResult = ::TerminateThread(m_RunThread.native_handle(), nExitCode) == TRUE;
	m_RunThread.detach();
	m_RunThread.swap(std::thread());
	return bResult;
}
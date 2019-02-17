#include "MessageQuene.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

bool CMessageQuene::InitIPC(const char* pIpcName, E_IPC_MODE nMode, size_t nMaxMsg, size_t nMaxMsgSize)
{
	bool bResult = false;
	if (NULL == pIpcName)
	{
		return false;
	}
	
	try
	{
		switch (nMode)
		{
		case E_IPC_MODE_SENDER:
			m_pMessageQueue = boost::make_shared<boost::interprocess::message_queue>(boost::interprocess::create_only, pIpcName, nMaxMsg, nMaxMsgSize);
			break;
		case E_IPC_MODE_RECEIVER:
			m_pMessageQueue = boost::make_shared<boost::interprocess::message_queue>(boost::interprocess::open_only, pIpcName);
			break;
		case E_IPC_MODE_BOTH:
			m_pMessageQueue = boost::make_shared<boost::interprocess::message_queue>(boost::interprocess::open_or_create, pIpcName, nMaxMsg, nMaxMsgSize);
			break;
		}
	}
	catch (...) {};
	return NULL != m_pMessageQueue;
}

bool CMessageQuene::UnInitIPC(const char* pIpcName)
{
	m_pMessageQueue.reset();
	boost::interprocess::message_queue::remove(pIpcName);
	return true;
}

bool CMessageQuene::Send(void* pBuffer, size_t nLength)
{
	if (NULL == m_pMessageQueue || NULL == pBuffer || 0 == nLength)
	{
		return false;
	}

	m_pMessageQueue->send(pBuffer, nLength, 0);
	return true;
}

bool CMessageQuene::TimeSend(void* pBuffer, size_t nLength, size_t nMillisecond)
{
	if (NULL == m_pMessageQueue || NULL == pBuffer || 0 == nLength)
	{
		return false;
	}
	
	boost::posix_time::ptime pTime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());
	boost::posix_time::milliseconds Offest((boost::int64_t)nMillisecond);
	return m_pMessageQueue->timed_send(pBuffer, nLength, 0, pTime + Offest);
}

bool CMessageQuene::Receive(void* pBuffer, size_t nLength)
{
	if (NULL == m_pMessageQueue || NULL == pBuffer || 0 == nLength)
	{
		return false;
	}
	
	size_t nReceiveSize = 0, nPriority = 0;
	m_pMessageQueue->receive(pBuffer, nLength, nReceiveSize, (unsigned int&)nPriority);
	return true;
}

bool CMessageQuene::TimeReceive(void* pBuffer, size_t nLength, size_t nMillisecond)
{
	if (NULL == m_pMessageQueue || NULL == pBuffer || 0 == nLength)
	{
		return false;
	}

	boost::posix_time::ptime pTime(boost::posix_time::from_time_t(time(NULL)));
	boost::posix_time::milliseconds Offest((boost::int64_t)nMillisecond);
	size_t nReceiveSize = 0, nPriority = 0;
	return m_pMessageQueue->timed_receive(pBuffer, nLength, nReceiveSize, (unsigned int&)nPriority, pTime + Offest);
}
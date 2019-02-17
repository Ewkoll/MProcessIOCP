#include "LockIMPL.h"

#ifdef _WIN32

CLockIMPL::CLockIMPL()
{
	InitializeCriticalSection(&_crisection);
}

CLockIMPL::~CLockIMPL()
{
	DeleteCriticalSection(&_crisection);
}

void CLockIMPL::Lock()
{
	EnterCriticalSection(&_crisection);
}

void CLockIMPL::UnLock()
{
	LeaveCriticalSection(&_crisection);
}

#else

CLockIMPL::CLockIMPL()
{
	pthread_mutexattr_init(&m_thread_mutexattr_t);
	pthread_mutexattr_settype(&m_thread_mutexattr_t, PTHREAD_MUTEX_TIMED_NP);
	pthread_mutex_init(&m_thread_mutex_t, &m_thread_mutexattr_t);
}

CLockIMPL::~CLockIMPL()
{
	pthread_mutexattr_destroy(&m_thread_mutexattr_t);
	pthread_mutex_destroy(&m_thread_mutex_t);
}

void CLockIMPL::Lock()
{
	pthread_mutex_lock(&m_thread_mutex_t);
}

void CLockIMPL::UnLock()
{
	pthread_mutex_unlock(&m_thread_mutex_t);
}

#endif
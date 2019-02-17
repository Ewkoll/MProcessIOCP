#include "KernelEventIMPL.h"

CKernelEventIMPL::CKernelEventIMPL(bool bManualReset, bool bInitialState, std::string strName /* = std::string() */)
	: m_hEvent(::CreateEventA(NULL, bManualReset == true, bInitialState == true, strName.c_str()))
{
	
}

CKernelEventIMPL::~CKernelEventIMPL()
{
	if (NULL != m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

bool CKernelEventIMPL::SetEvent()
{
	return ::SetEvent(m_hEvent) == TRUE;
}

bool CKernelEventIMPL::ResetEvent()
{
	return ::ResetEvent(m_hEvent) == TRUE;
}

HANDLE CKernelEventIMPL::GetNativeHandle()
{
	return m_hEvent;
}

DWORD CKernelEventIMPL::WaitForSingleObject(DWORD dwMilliseconds)
{
	return ::WaitForSingleObject(m_hEvent, dwMilliseconds);
}
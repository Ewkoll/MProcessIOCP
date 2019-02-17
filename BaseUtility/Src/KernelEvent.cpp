#include "KernelEvent.h"
#include "KernelEventIMPL.h"

CKernelEvent::CKernelEvent(bool bManualReset, bool bInitialState, std::string strName /*= std::string()*/)
	: m_pIMPL(new CKernelEventIMPL(bManualReset, bInitialState, strName))
{

}

CKernelEvent::~CKernelEvent()
{
	if (NULL != m_pIMPL)
	{
		delete m_pIMPL;
		m_pIMPL = NULL;
	}
}

bool CKernelEvent::SetEvent()
{
	return m_pIMPL->SetEvent();
}

bool CKernelEvent::ResetEvent()
{
	return m_pIMPL->ResetEvent();
}

HANDLE CKernelEvent::GetNativeHandle()
{
	return m_pIMPL->GetNativeHandle();
}

DWORD CKernelEvent::WaitForSingleObject(DWORD dwMilliseconds)
{
	return m_pIMPL->WaitForSingleObject(dwMilliseconds);
}
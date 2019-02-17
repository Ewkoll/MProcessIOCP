#include "Lock.h"
#include "LockIMPL.h"

CLock::CLock()
	: m_pIMPL(new CLockIMPL)
{

}

CLock::~CLock()
{
	delete m_pIMPL;
}

void CLock::Lock()
{
	m_pIMPL->Lock();
}

void CLock::UnLock()
{
	m_pIMPL->UnLock();
}
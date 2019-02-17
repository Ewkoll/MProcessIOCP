#include "WorkPath.h"
#include "WorkPathIMPL.h"

CWorkPath::CWorkPath()
	: m_pIMPL(new CWorkPathIMPL)
{
	m_pIMPL->GetCurWorkPath();
}

CWorkPath::CWorkPath(const std::string& strPath)
	: m_pIMPL(new CWorkPathIMPL(strPath))
{
	m_pIMPL->GetCurWorkPath();
	m_pIMPL->SetWorkPath(strPath);
}

CWorkPath::~CWorkPath()
{
	if (NULL != m_pIMPL)
	{
		delete m_pIMPL;
		m_pIMPL = NULL;
	}
}

bool CWorkPath::SetWorkPath(const std::string& strPath)
{
	return m_pIMPL->SetWorkPath(strPath);
}

std::string CWorkPath::GetModulePath()
{
	return m_pIMPL->GetModulePath();
}

std::string CWorkPath::GetCurWorkPath()
{
	return m_pIMPL->GetCurWorkPath();
}
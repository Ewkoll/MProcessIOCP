#include "Library.h"

CLibrary::CLibrary()
{
	m_hModule = NULL;
}

CLibrary::~CLibrary()
{
	FreeLibrary();
}

bool CLibrary::IsLoadSuccess()
{
	return NULL != m_hModule;
}

bool CLibrary::LoadLibrary(const std::string& strPath)
{
	HINSTANCE hModule = ::LoadLibraryExA(strPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);;
	if (NULL == hModule)
	{
		return false;
	}

	m_hModule = hModule;
	return true;
}

void CLibrary::FreeLibrary()
{
	if (NULL != m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}
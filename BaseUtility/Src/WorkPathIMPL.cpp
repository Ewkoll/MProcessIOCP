#include "WorkPathIMPL.h"
#include "Common\Global.h"
#include <memory>
#include <Windows.h>

CWorkPathIMPL::CWorkPathIMPL()
{
	GetCurWorkPath();
}

CWorkPathIMPL::CWorkPathIMPL(const std::string& strPath)
{
	GetCurWorkPath();
	SetWorkPath(strPath);
}

CWorkPathIMPL::~CWorkPathIMPL()
{
	SetCurrentDirectoryA(m_strSrcWorkPath.c_str());
}

bool CWorkPathIMPL::SetWorkPath(const std::string& strPath)
{
	if (strPath.empty())
	{
		return false;
	}

	if (SetCurrentDirectoryA(strPath.c_str()))
	{
		m_strCurWorkPath = strPath;
		return true;
	}
	return false;
}

std::string CWorkPathIMPL::GetModulePath()
{
	CHAR szLogDirectory[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szLogDirectory, MAX_PATH);
	CHAR *pItem = STRRCHR(szLogDirectory, '\\') + 1;
	if (NULL != pItem)
	{
		*(pItem) = '\0';
		return std::string(szLogDirectory);
	}
	return std::string();
}

std::string CWorkPathIMPL::GetCurWorkPath()
{
	CHAR cCurDir[MAX_PATH] = { 0 };
	std::string strResult;
	DWORD dwRet = GetCurrentDirectoryA(MAX_PATH, cCurDir);
	do 
	{
		if (0 == dwRet)
		{
			break;
		}

		if (dwRet <= MAX_PATH)
		{
			strResult = cCurDir;
			m_strSrcWorkPath = strResult;
			break;
		}

		if (dwRet > MAX_PATH)
		{
			std::shared_ptr<CHAR> pBuffer(new CHAR[dwRet + 1]);
			ZeroMemory(pBuffer.get(), dwRet + 1);
			DWORD dwResult = GetCurrentDirectoryA(dwRet + 1, pBuffer.get());
			if (dwResult == dwRet)
			{
				strResult = pBuffer.get();
				m_strSrcWorkPath = strResult;
			}
			break;
		}
	} while (false);

	char cLast = strResult[strResult.length() - 1];
	if (cLast != '\\' || cLast != '/')
	{
		strResult += '\\';
	}
	return strResult;
}
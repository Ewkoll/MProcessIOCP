#include "FileLogIMPL.h"

std::string CFileLogIMPL::m_strFilePathName;
extern "C" IMAGE_DOS_HEADER __ImageBase;

CFileLogIMPL::CFileLogIMPL()
	: m_pFile(NULL)
{
	SetLogName("");
}

CFileLogIMPL::~CFileLogIMPL()
{
	CloseLogFile();
}

HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300
	MEMORY_BASIC_INFORMATION MemoryBaseINFO;
	static int nDummy = 0;
	VirtualQuery(&nDummy, &MemoryBaseINFO, sizeof(MemoryBaseINFO));
	return reinterpret_cast<HMODULE>(MemoryBaseINFO.AllocationBase);
#else
	return reinterpret_cast<HMODULE>(&__ImageBase);
#endif  
}

void CFileLogIMPL::SetLogFilePath()
{
	CHAR szLogDirectory[MAX_PATH] = { 0 };
	GetModuleFileNameA(GetCurrentModule(), szLogDirectory, MAX_PATH);
	try
	{
		CHAR *pItem = STRRCHR(szLogDirectory, '\\') + 1;
		if (NULL != pItem)
		{
			m_strModuleName = pItem;
			*(pItem) = '\0';
			STRCAT(szLogDirectory, sizeof(szLogDirectory), "Log\\");
		}
		CreateDirectoryA(szLogDirectory, NULL);
	} catch (...){}
	m_strFilePathName = szLogDirectory;
}

void CFileLogIMPL::CloseLogFile()
{
	if (NULL != m_pFile)
	{
		fflush(m_pFile);
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void CFileLogIMPL::SetLogName(const std::string& strLogName)
{
	CLog::SetLogName(strLogName);
	if (m_strFilePathName.empty())
	{
		SetLogFilePath();
	}

	SYSTEMTIME SystemTime;
	CHAR szDate[MAX_PATH] = { 0 };
	GetLocalTime(&SystemTime);
	SPRINTF(szDate, COUNTOF(szDate), "%04d_%02d_%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);
	
	CHAR szLogPath[MAX_PATH] = { 0 };
	if (strLogName.empty())
	{
		SPRINTF(szLogPath, COUNTOF(szLogPath), "%s\\Log_%s.txt", m_strFilePathName.c_str(), szDate);
	}
	else
	{
		SPRINTF(szLogPath, COUNTOF(szLogPath), "%s\\Log_%s_%s.txt", m_strFilePathName.c_str(), szDate, strLogName.c_str());
	}
	m_strLogPath = szLogPath;
}

void CFileLogIMPL::WriteMSG(const std::string& strLevelName, const std::string& strLog)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	CHAR szDate[MAX_PATH] = { 0 };
	SPRINTF(szDate, COUNTOF(szDate), " %02d:%02d:%02d ", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
	
	std::string strLogData = strLevelName + szDate + strLog;
	if (strLog[strLog.length() - 1] != '\n')
	{
		strLogData += "\r\n";
	}

	bool bResult = false;
	do 
	{
		if (m_strLogPath.empty())
		{
			break;
		}

		if (NULL == m_pFile)
		{
			m_pFile = fopen(m_strLogPath.c_str(), "w+");
			if (NULL == m_pFile)
			{
				break;
			}
		}
		
		fwrite(strLogData.c_str(), 1, strLogData.length(), m_pFile);
		fflush(m_pFile);
		bResult = true;
	} while (false);

	if (!bResult) { OutputDebugStringA(strLogData.c_str()); }
}
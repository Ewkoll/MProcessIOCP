#include "FileLog.h"
#include "FileLogIMPL.h"

CFileLog::CFileLog()
	:m_pLog(new CFileLogIMPL)
{

}

CFileLog::~CFileLog()
{
	if (NULL != m_pLog)
	{
		delete m_pLog;
		m_pLog = NULL;
	}
}

void CFileLog::SetLogLevel(int nLevel)
{
	m_pLog->SetLogLevel(nLevel);
}

E_LOG_LEVEL CFileLog::GetLogLevel()
{
	return m_pLog->GetLogLevel();
}

void CFileLog::SetLogName(const std::string& strLogName)
{
	m_pLog->SetLogName(strLogName);
}

std::string CFileLog::GetLogName()
{
	return m_pLog->GetLogName();
}

std::string CFileLog::Format(const char *pFormat, va_list& ap)
{
	return m_pLog->Format(pFormat, ap);
}

void CFileLog::DebugMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_DEBUG))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("Debug", Format(pFormat, ap));
	}
}

void CFileLog::InfoMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_INFO))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("INFO", Format(pFormat, ap));
	}
}

void CFileLog::ErrorMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_ERROR))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("Error", Format(pFormat, ap));
	}
}

void CFileLog::WriteMSG(const std::string& strLevelName, const std::string& strLog)
{
	m_pLog->WriteMSG(strLevelName, strLog);
}
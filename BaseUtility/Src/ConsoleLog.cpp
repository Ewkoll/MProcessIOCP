#include "ConsoleLog.h"
#include "ConsoleLogIMPL.h"

CConsoleLog::CConsoleLog()
	:m_pLog(new CConsoleLogIMPL)
{

}

CConsoleLog::~CConsoleLog()
{
	if (NULL != m_pLog)
	{
		delete m_pLog;
		m_pLog = NULL;
	}
}

void CConsoleLog::SetLogLevel(int nLevel)
{
	m_pLog->SetLogLevel(nLevel);
}

E_LOG_LEVEL CConsoleLog::GetLogLevel()
{
	return m_pLog->GetLogLevel();
}

void CConsoleLog::SetLogName(const std::string& strLogName)
{
	m_pLog->SetLogName(strLogName);
}

std::string CConsoleLog::GetLogName()
{
	return m_pLog->GetLogName();
}

std::string CConsoleLog::Format(const char *pFormat, va_list& ap)
{
	return m_pLog->Format(pFormat, ap);
}

void CConsoleLog::DebugMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_DEBUG))
	{
		va_list ap;
		va_start(ap, pFormat);
		Write("DEBUG", Format(pFormat, ap));
	}
}

void CConsoleLog::InfoMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_INFO))
	{
		va_list ap;
		va_start(ap, pFormat);
		Write("INFO", Format(pFormat, ap));
	}
}

void CConsoleLog::ErrorMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (GetLogLevel() & E_LOG_LEVEL_ERROR))
	{
		va_list ap;
		va_start(ap, pFormat);
		Write("ERROR", Format(pFormat, ap));
	}
}

void CConsoleLog::Write(const std::string& strLevelName, const std::string& strLog)
{
	m_pLog->WriteMSG(strLevelName, strLog);
}
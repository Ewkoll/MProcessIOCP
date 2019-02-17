#include "DebugLog.h"
#include "ILog.h"
#include "FileLog.h"
#include "ConsoleLog.h"
#include "Common/Sington.h"

class ConsoleLog : public Singleton<ConsoleLog>
{
public:
	ConsoleLog()
	{
		m_pConsoleLog = std::shared_ptr<CConsoleLog>(new CConsoleLog);
		m_pFileLog = std::shared_ptr<CFileLog>(new CFileLog);
		m_pFileLog->SetLogLevel(E_LOG_LEVEL_ERROR | E_LOG_LEVEL_INFO);
		m_nPID = GetCurrentProcessId();
	}

public:
	int m_nPID;
	std::shared_ptr<CConsoleLog> m_pConsoleLog;
	std::shared_ptr<CFileLog> m_pFileLog;
};

void DebugFileLog(const char *pFormat, ...)
{
	std::shared_ptr<CFileLog> pLog = ConsoleLog::GetInstance().m_pFileLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_DEBUG)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->DebugMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}

void InfoFileLog(const char *pFormat, ...)
{
	std::shared_ptr<CFileLog> pLog = ConsoleLog::GetInstance().m_pFileLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_INFO)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->InfoMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}

void ErrorFileLog(const char *pFormat, ...)
{
	std::shared_ptr<CFileLog> pLog = ConsoleLog::GetInstance().m_pFileLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_ERROR)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->ErrorMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}

void DebugConsoleLog(const char *pFormat, ...)
{
	std::shared_ptr<CConsoleLog> pLog = ConsoleLog::GetInstance().m_pConsoleLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_DEBUG)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->DebugMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}

void InfoConsoleLog(const char *pFormat, ...)
{
	std::shared_ptr<CConsoleLog> pLog = ConsoleLog::GetInstance().m_pConsoleLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_INFO)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->InfoMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}

void ErrorConsoleLog(const char *pFormat, ...)
{
	std::shared_ptr<CConsoleLog> pLog = ConsoleLog::GetInstance().m_pConsoleLog;
	if (pLog->GetLogLevel() & E_LOG_LEVEL_ERROR)
	{
		int nPID = ConsoleLog::GetInstance().m_nPID;
		va_list ap;
		va_start(ap, pFormat);
		pLog->ErrorMSG("PID:[%d] %s", nPID, pLog->Format(pFormat, ap).c_str());
	}
}
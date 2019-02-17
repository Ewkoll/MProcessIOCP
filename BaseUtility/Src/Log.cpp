#include "Log.h"
#include <vector>
#include "Common/Global.h"

CLog::CLog()
	: m_nLevel(E_LOG_LEVEL_ALL)
{

}

CLog::~CLog()
{

}

void CLog::SetLogLevel(int nLevel)
{
	m_nLevel = (E_LOG_LEVEL)nLevel;
}

E_LOG_LEVEL CLog::GetLogLevel()
{
	return m_nLevel;
}

void CLog::SetLogName(const std::string& strLogName)
{
	m_strLogName = strLogName;
}

std::string CLog::GetLogName()
{
	return m_strLogName;
}

void CLog::DebugMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (m_nLevel & E_LOG_LEVEL_DEBUG))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("DEBUG", Format(pFormat, ap));
	}
}

void CLog::InfoMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (m_nLevel & E_LOG_LEVEL_INFO))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("INFO", Format(pFormat, ap));
	}
}

void CLog::ErrorMSG(const char *pFormat, ...)
{
	if (NULL != pFormat && (m_nLevel & E_LOG_LEVEL_ERROR))
	{
		va_list ap;
		va_start(ap, pFormat);
		WriteMSG("ERROR", Format(pFormat, ap));
	}
}

std::string CLog::Format(const char *pFormat, va_list& ap)
{
	std::vector<char> vBuffer(1024);
	int nResult = VSNPRINTF(&vBuffer[0], vBuffer.size() - 1, vBuffer.size() - 1, pFormat, ap);
	if (nResult >= (int)vBuffer.size())
	{
		vBuffer.resize(nResult);
		VSNPRINTF(&vBuffer[0], vBuffer.size() - 1, vBuffer.size() - 1, pFormat, ap);
	}
	va_end(ap);

	vBuffer[vBuffer.size() - 1] = '\0';
	return std::string(&vBuffer[0]);
}
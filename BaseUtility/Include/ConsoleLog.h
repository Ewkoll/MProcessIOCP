/*****************************************************************************
*  @file     ConsoleLog.h
*  @brief    控制台日记导出接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 11:13:40
*****************************************************************************/
#ifndef ConsoleLog_h__
#define ConsoleLog_h__

#pragma once
#include "ILog.h"

class DLL_EXPORT CConsoleLog final
{
public:

	CConsoleLog();

	virtual ~CConsoleLog();

	CConsoleLog(const CConsoleLog& Other) = delete;

	CConsoleLog& operator=(const CConsoleLog& Other) = delete;

	void SetLogLevel(int nLevel);

	E_LOG_LEVEL GetLogLevel();

	void SetLogName(const std::string& strLogName);

	std::string GetLogName();

	std::string Format(const char *pFormat, va_list& ap);

	void DebugMSG(const char *pFormat, ...);

	void InfoMSG(const char *pFormat, ...);

	void ErrorMSG(const char *pFormat, ...);

	void Write(const std::string& strLevelName, const std::string& strLog);

protected:

	ILog *m_pLog;	///< 具体的LOG实现指针。
};

#endif // ConsoleLog_h__
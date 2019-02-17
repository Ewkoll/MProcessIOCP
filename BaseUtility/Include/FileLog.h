/*****************************************************************************
*  @file     FileLog.h
*  @brief    文本日记导出接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 11:13:40
*****************************************************************************/
#ifndef FileLog_h__
#define FileLog_h__

#pragma once
#include "ILog.h"

class DLL_EXPORT CFileLog final
{
public:

	CFileLog();

	virtual ~CFileLog();

	CFileLog(const CFileLog& Other) = delete;

	CFileLog& operator=(const CFileLog& Other) = delete;

	void SetLogLevel(int nLevel);

	E_LOG_LEVEL GetLogLevel();

	void SetLogName(const std::string& strLogName);

	std::string GetLogName();

	std::string Format(const char *pFormat, va_list& ap);

	void DebugMSG(const char *pFormat, ...);

	void InfoMSG(const char *pFormat, ...);

	void ErrorMSG(const char *pFormat, ...);

	void WriteMSG(const std::string& strLevelName, const std::string& strLog);

protected:

	ILog *m_pLog;	///< 具体的LOG实现指针。
};

#endif // FileLog_h__
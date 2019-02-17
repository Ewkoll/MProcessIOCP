/*****************************************************************************
*  @file     ILog.h
*  @brief    Log–Èª˘¿‡°£
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/04/28 14:04:01
*****************************************************************************/
#ifndef ILog_h__
#define ILog_h__

#pragma once
#include "Common/IObject.h"

enum E_LOG_LEVEL
{
	E_LOG_LEVEL_DEBUG = 0x01,
	E_LOG_LEVEL_INFO = 0x02,
	E_LOG_LEVEL_ERROR = 0x04,
	E_LOG_LEVEL_ALL = 0x07,
};

class ILog
{
public:

	virtual ~ILog() {};

	virtual void SetLogLevel(int nLevel) = 0;

	virtual E_LOG_LEVEL GetLogLevel() = 0;

	virtual void SetLogName(const std::string& strLogName) = 0;

	virtual std::string GetLogName() = 0;

	virtual std::string Format(const char *pFormat, va_list& ap) = 0;
		
	virtual void DebugMSG(const char *pFormat, ...) = 0;

	virtual void InfoMSG(const char *pFormat, ...) = 0;

	virtual void ErrorMSG(const char *pFormat, ...) = 0;

	virtual void WriteMSG(const std::string& strLevelName, const std::string& strLog) = 0;
};

#endif // ILog_h__
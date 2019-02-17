/*****************************************************************************
*  @file     Log.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/04/28 15:30:01
*****************************************************************************/
#ifndef Log_h__
#define Log_h__

#pragma once
#include "ILog.h"

class CLog : public ILog
{
public:
		
	CLog();

	virtual ~CLog();

	virtual void SetLogLevel(int nLevel) override;

	virtual E_LOG_LEVEL GetLogLevel() override;

	virtual void SetLogName(const std::string& strLogName) override;

	virtual std::string GetLogName() override;

	virtual std::string Format(const char *pFormat, va_list& ap);

	virtual void DebugMSG(const char *pFormat, ...) override;

	virtual void InfoMSG(const char *pFormat, ...) override;

	virtual void ErrorMSG(const char *pFormat, ...) override;
		
protected:
	E_LOG_LEVEL m_nLevel;		///< 日记的等级。
	std::string m_strLogName;	///< 日记类型名称。
};

#endif // Log_h__
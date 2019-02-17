/*****************************************************************************
*  @file     FileLogIMPL.h
*  @brief    文件LOG实现。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/05/04 16:22:39
*****************************************************************************/
#ifndef FileLogIMPL_h__
#define FileLogIMPL_h__

#pragma once
#include "Log.h"

class CFileLogIMPL : public CLog
{
public:

	CFileLogIMPL();

	virtual ~CFileLogIMPL();

	virtual void WriteMSG(const std::string& strLevelName, const std::string& strLog) override;

	virtual void SetLogName(const std::string& strLogName) override;

private:

	void SetLogFilePath();

	void CloseLogFile();

private:
	FILE *m_pFile;
	std::string m_strLogPath;
	std::string m_strModuleName;
	static std::string m_strFilePathName;
};

#endif // FileLogIMPL_h__
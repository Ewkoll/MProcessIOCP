/*****************************************************************************
*  @file     ConsoleLogIMPL.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/04/28 15:24:19
*****************************************************************************/
#ifndef ConsoleLogIMPL_h__
#define ConsoleLogIMPL_h__

#pragma once
#include "Log.h"

class CConsoleLogIMPL : public CLog
{
public:

	CConsoleLogIMPL();

	virtual ~CConsoleLogIMPL();

	virtual void WriteMSG(const std::string& strLevelName, const std::string& strLog) override;

protected:
	bool m_bInit;				///< ³õÊ¼»¯×´Ì¬¡£
	HANDLE m_hOutput;			///< Êä³ö¾ä±ú¡£
};

#endif // ConsoleLogIMPL_h__
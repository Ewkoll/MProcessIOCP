/*****************************************************************************
*  @file     Library.h
*  @brief    载入库
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/21 09:51:02
*****************************************************************************/
#ifndef Library_h__
#define Library_h__

#pragma once
#include <string>
#include "Common/Global.h"

/**
* @brief WIN32 和 Linux上的载入DLL有差别，封装到一个类中完成。
*/
class DLL_EXPORT CLibrary
{
public:
	
	CLibrary();
	
	virtual ~CLibrary();
	
	bool IsLoadSuccess();

	bool LoadLibrary(const std::string& strPath);

	void FreeLibrary();

	LIB_MODULE GetModule() { return m_hModule; };

private:

	LIB_MODULE m_hModule;	///< 存储载入的DLL库。
};

#endif // Library_h__
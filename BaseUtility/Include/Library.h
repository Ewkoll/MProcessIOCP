/*****************************************************************************
*  @file     Library.h
*  @brief    �����
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
* @brief WIN32 �� Linux�ϵ�����DLL�в�𣬷�װ��һ��������ɡ�
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

	LIB_MODULE m_hModule;	///< �洢�����DLL�⡣
};

#endif // Library_h__
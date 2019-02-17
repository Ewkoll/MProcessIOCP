/*****************************************************************************
*  @file     Convert.h
*  @brief    转换导出接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 13:36:56
*****************************************************************************/
#ifndef Convert_h__
#define Convert_h__

#pragma once
#include "Common/IObject.h"

class CConvertIMPL;

class DLL_EXPORT CConvert final
{
public:
	
	CConvert();
	
	virtual ~CConvert();
	
	CConvert(const CConvert& Other) = delete;

	CConvert& operator=(const CConvert& Other) = delete;

	bool StringToUTF8String(std::string& strInput, std::string& strOutput);

	bool UTF8StringToString(std::string& strInput, std::string& strOutput);

	bool WideStringToString(std::wstring& strInput, std::string& strOutput);

	bool StringToWideString(std::string& strInput, std::wstring& strOutput);

private:

	CConvertIMPL *m_pIMPL;	///< 具体的转换类实现指针。
};

#endif // Convert_h__
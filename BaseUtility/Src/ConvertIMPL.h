/*****************************************************************************
*  @file     ConvertIMPL.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/05/05 15:54:49
*****************************************************************************/
#ifndef ConvertIMPL_h__
#define ConvertIMPL_h__

#pragma once
#include <string>

class CConvertIMPL
{
public:

	bool StringToUTF8String(std::string& strInput, std::string& strOutput);

	bool UTF8StringToString(std::string& strInput, std::string& strOutput);

	bool WideStringToString(std::wstring& strInput, std::string& strOutput);

	bool StringToWideString(std::string& strInput, std::wstring& strOutput);
};

#endif // ConvertIMPL_h__
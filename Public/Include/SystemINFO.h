/*****************************************************************************
*  @file     SystemINFO.h
*  @brief    系统信息。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/06/08 10:32:24
*****************************************************************************/
#ifndef SystemINFO_h__
#define SystemINFO_h__

#pragma once
#include "Windows.h"

class CSystemINFO
{
public:
	/*
	 *	获取CPU数量。
	 */
	static DWORD GetNumberOfProcessors();
};

#endif // SystemINFO_h__
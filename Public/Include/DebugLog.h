/*****************************************************************************
*  @file     DebugLog.h
*  @brief    全局LOG。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-02 15:40:02
*****************************************************************************/
#ifndef DebugLog_h__
#define DebugLog_h__

#pragma once
#include <string>

void DebugFileLog(const char *pFormat, ...);

void InfoFileLog(const char *pFormat, ...);

void ErrorFileLog(const char *pFormat, ...);

void DebugConsoleLog(const char *pFormat, ...);

void InfoConsoleLog(const char *pFormat, ...);

void ErrorConsoleLog(const char *pFormat, ...);

#endif // DebugLog_h__
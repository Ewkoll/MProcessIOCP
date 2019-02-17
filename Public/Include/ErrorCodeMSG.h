/*****************************************************************************
*  @file     ErrorCodeMSG.h
*  @brief    错误信息。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-02 15:29:58
*****************************************************************************/
#ifndef ErrorCodeMSG_h__
#define ErrorCodeMSG_h__

#pragma once
#include <string>

std::string ErrorCode2Text(DWORD dwErrorCode);

std::string ErrorCode2TextV2(DWORD dwErrorCode, std::string strFile = __FILE__, int nLineCount = __LINE__);

#endif // ErrorCodeMSG_h__
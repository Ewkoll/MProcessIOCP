/*****************************************************************************
*  @file     KernelEventIMPL.h
*  @brief    ϵͳEvent
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/06/09 10:48:46
*****************************************************************************/
#ifndef KernelEventIMPL_h__
#define KernelEventIMPL_h__

#pragma once
#include <string>
#include <Windows.h>

class CKernelEventIMPL
{
public:

	CKernelEventIMPL(bool bManualReset, bool bInitialState, std::string strName = std::string());

	~CKernelEventIMPL();

	bool SetEvent();

	bool ResetEvent();

	HANDLE GetNativeHandle();
		
	DWORD WaitForSingleObject(DWORD dwMilliseconds);

protected:

	HANDLE m_hEvent;	///< Window �¼������
};

#endif // KernelEventIMPL_h__
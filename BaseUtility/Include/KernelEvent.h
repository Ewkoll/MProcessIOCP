/*****************************************************************************
*  @file     KernelEvent.h
*  @brief    �ں��¼�������
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 13:48:05
*****************************************************************************/
#ifndef KernelEvent_h__
#define KernelEvent_h__

#pragma once
#include "Common/IObject.h"

class CKernelEventIMPL;

class DLL_EXPORT CKernelEvent final
{
public:
	
	CKernelEvent(bool bManualReset, bool bInitialState, std::string strName = std::string());
	
	virtual ~CKernelEvent();
	
	CKernelEvent(const CKernelEvent& Other) = delete;

	CKernelEvent& operator=(const CKernelEvent& Other) = delete;

	bool SetEvent();

	bool ResetEvent();

	HANDLE GetNativeHandle();

	DWORD WaitForSingleObject(DWORD dwMilliseconds);

private:

	CKernelEventIMPL *m_pIMPL;	///< ������ں��¼�ʵ�ֶ���
};

#endif // KernelEvent_h__
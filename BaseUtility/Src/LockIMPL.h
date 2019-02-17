/*****************************************************************************
*  @file     LockIMPL.h
*  @brief    此对象不允许拷贝构造。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/05/05 15:33:36
*****************************************************************************/
#ifndef LockIMPL_h__
#define LockIMPL_h__
#pragma once

#include "Common/Global.h"

#ifdef _WIN32

class CLockIMPL
{
public:
	
	CLockIMPL();
	
	virtual ~CLockIMPL();
	
	CLockIMPL(const CLockIMPL& Other) = delete;

	CLockIMPL& operator=(const CLockIMPL& Other) = delete;

	void Lock();
	
	void UnLock();

private:
	CRITICAL_SECTION _crisection;
};

#else

class CLockIMPL
{
public:
	CLockIMPL();

	virtual ~CLockIMPL();

	CLockIMPL(const CLockIMPL& Other) = delete;

	CLockIMPL& operator=(const CLockIMPL& Other) = delete;

	void Lock();
	
	void UnLock();

private:
	pthread_mutexattr_t m_thread_mutexattr_t;
	pthread_mutex_t m_thread_mutex_t;
};

#endif

#endif // LockIMPL_h__
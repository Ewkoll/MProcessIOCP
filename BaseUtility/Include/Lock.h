/*****************************************************************************
*  @file     Lock.h
*  @brief    Lock导出接口。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-07 13:43:22
*****************************************************************************/
#ifndef Lock_h__
#define Lock_h__

#pragma once
#include "Common/IObject.h"

class CLockIMPL;

class DLL_EXPORT CLock final
{
public:

	CLock();

	virtual ~CLock();

	CLock(const CLock& Other) = delete;

	CLock& operator=(const CLock& Other) = delete;

	void Lock();

	void UnLock();

private:

	CLockIMPL *m_pIMPL;	///< 具体的锁定对象。
};

namespace Common
{
	class CLockHelper
	{
	public:
		CLockHelper(CLock& Lock) : m_Lock(Lock) { m_Lock.Lock(); };
		~CLockHelper() { m_Lock.UnLock(); };

	protected:
		CLockHelper(const CLockHelper& Other) = delete;
		CLockHelper& operator=(const CLockHelper& Other) = delete;

	private:
		CLock& m_Lock;
	};
}

#endif // Lock_h__
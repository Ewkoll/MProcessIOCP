/*****************************************************************************
*  @file     IOCPBufferManager.h
*  @brief    缓冲区管理
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-03 17:17:41
*****************************************************************************/
#ifndef IOCPBufferManager_h__
#define IOCPBufferManager_h__

#pragma once
#include <list>
#include "Lock.h"
#include "IOCPBuffer.h"
#include "Common/Sington.h"
#include "HashMap/HashMap.h"

namespace IOCP
{
	class CIOCPBufferManager
	{
	public:
		
		CIOCPBufferManager(const DWORD dwCacheCount = E_BUFFER_CACHE_SIZE);

		~CIOCPBufferManager();

		enum { E_BUFFER_CACHE_SIZE = 1024 * 10 };

		bool ReleaseBuffer(CIOCPBuffer *pIOCPBuffer);

		CIOCPBuffer* AllocateBuffer(int nType, int nLength = IOBUFFER_DATA_LENGTH);

	protected:

		CIOCPBuffer* GetFromCache();

		void InitCache();

		void FreeAllBuffer();

		bool FreeIOBuffer(CIOCPBuffer *pIOCPBuffer);
		
		void InternalReleaseBufferCallBack(CIOCPBuffer *pIOCPBuffer);

		static void ReleaseBufferCallBack(CIOCPBuffer *pIOCPBuffer);

		static void ReleaseCacheBufferCallBack(CIOCPBuffer *pIOCPBuffer);
		
	protected:
		HashMAP::CHashMap<CIOCPBuffer*, ULONG_PTR, 32> m_CacheBuffer;	///< 基于线程的缓存，最大32个线程。大于线程数，取数据会冲突。
		bool m_bEnableCache;											///< 是否开始缓存。
		const DWORD m_dwCacheCount;										///< Cache的大小。
		volatile bool m_bStop;											///< 停止Cache标记。
		HashMAP::CHashMap<CIOCPBuffer*, ULONG_PTR> m_HashBuffer;		///< 存储IOBUFFER指针。
	};
}

#endif // IOCPBufferManager_h__
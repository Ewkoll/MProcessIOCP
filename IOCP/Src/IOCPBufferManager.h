/*****************************************************************************
*  @file     IOCPBufferManager.h
*  @brief    ����������
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
		HashMAP::CHashMap<CIOCPBuffer*, ULONG_PTR, 32> m_CacheBuffer;	///< �����̵߳Ļ��棬���32���̡߳������߳�����ȡ���ݻ��ͻ��
		bool m_bEnableCache;											///< �Ƿ�ʼ���档
		const DWORD m_dwCacheCount;										///< Cache�Ĵ�С��
		volatile bool m_bStop;											///< ֹͣCache��ǡ�
		HashMAP::CHashMap<CIOCPBuffer*, ULONG_PTR> m_HashBuffer;		///< �洢IOBUFFERָ�롣
	};
}

#endif // IOCPBufferManager_h__
#include "IOCPBufferManager.h"
#include "DebugLog.h"
using namespace IOCP;

CIOCPBufferManager::CIOCPBufferManager(const DWORD dwCacheCount /*= E_BUFFER_CACHE_SIZE*/)
	: m_bStop(false)
	, m_bEnableCache(true)
	, m_dwCacheCount(dwCacheCount)
	, m_HashBuffer(HashMAP::HashValue)
{
	
}

CIOCPBufferManager::~CIOCPBufferManager()
{
	m_bStop = true;
	FreeAllBuffer();
}

bool CIOCPBufferManager::ReleaseBuffer(CIOCPBuffer *pIOCPBuffer)
{
	return m_HashBuffer.RemoveOnlyOneNode((ULONG_PTR)pIOCPBuffer);
}

void CIOCPBufferManager::FreeAllBuffer()
{
	m_HashBuffer.FreeHashMapNode();
	m_CacheBuffer.FreeHashMapNode();
}

CIOCPBuffer* CIOCPBufferManager::AllocateBuffer(int nType, int nLength /* = IOBUFFER_DATA_LENGTH */)
{
	CIOCPBuffer *pIOCPBuffer = GetFromCache();
	if (NULL == pIOCPBuffer)
	{
		pIOCPBuffer = new CIOCPBuffer(*this, nLength);
	}
	
	pIOCPBuffer->Init(nLength);
	pIOCPBuffer->SetOperation(nType);
	m_HashBuffer.AddNode((ULONG_PTR)pIOCPBuffer, pIOCPBuffer, ReleaseBufferCallBack);
	return pIOCPBuffer;
}

CIOCPBuffer* CIOCPBufferManager::GetFromCache()
{
	if (!m_bEnableCache)
	{
		return NULL;
	}

	auto Node = m_CacheBuffer.RemoveNode();
	if (NULL == Node)
	{
		return NULL;
	}
	return Node->GetData();
}

void CIOCPBufferManager::InitCache()
{
	int nCache = m_dwCacheCount;
	while (--nCache >= 0)
	{
		CIOCPBuffer *pIOCPBuffer = new CIOCPBuffer(*this);
		pIOCPBuffer->Init();
		pIOCPBuffer->SetOperation(OP_NULL);
		m_CacheBuffer.AddNode((ULONG_PTR)pIOCPBuffer, pIOCPBuffer, ReleaseCacheBufferCallBack);
	}
}

bool CIOCPBufferManager::FreeIOBuffer(CIOCPBuffer *pIOCPBuffer)
{
	if (NULL != pIOCPBuffer)
	{
		delete pIOCPBuffer;
		pIOCPBuffer = NULL;
	}
	return true;
}

void CIOCPBufferManager::InternalReleaseBufferCallBack(CIOCPBuffer *pIOCPBuffer)
{
	bool bFreeData = true;
	do
	{
		if (m_bStop)
		{
			break;
		}
		else
		{
			size_t nCount = m_CacheBuffer.GetSize();
			if (nCount < E_BUFFER_CACHE_SIZE)
			{
				bFreeData = false;
				m_CacheBuffer.AddNode((ULONG_PTR)pIOCPBuffer, pIOCPBuffer, ReleaseCacheBufferCallBack);
			}
		}
	} while (false);

	if (bFreeData)
	{
		FreeIOBuffer(pIOCPBuffer);
	}
}

void CIOCPBufferManager::ReleaseBufferCallBack(CIOCPBuffer *pIOCPBuffer)
{
	CIOCPBufferManager& rThis = pIOCPBuffer->GetBufferManager();
	if (!rThis.m_bEnableCache)
	{
		rThis.FreeIOBuffer(pIOCPBuffer);
	}
	else
	{
		rThis.InternalReleaseBufferCallBack(pIOCPBuffer);
	}
}

void CIOCPBufferManager::ReleaseCacheBufferCallBack(CIOCPBuffer *pIOCPBuffer)
{
	CIOCPBufferManager& rThis = pIOCPBuffer->GetBufferManager();
	rThis.FreeIOBuffer(pIOCPBuffer);
}
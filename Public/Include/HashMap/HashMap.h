/*****************************************************************************
*  @file     HashMap.h
*  @brief    哈希表
*  Details.                                                                 
*                                                                           
*  @author   IDeath
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-18 10:22:53
*****************************************************************************/
#pragma once
#include "List.h"
#include <mutex>
#include <vector>
#include <atomic>

namespace HashMAP
{
	#define DEFAULT_HASH_MAP_INITSIZE 1024
	
	template<bool use32> struct _hash_32_or_64 {
		static inline std::uint32_t hash(std::uint32_t h)
		{
			// MurmurHash3 finalizer -- see https://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp
			// Since the thread ID is already unique, all we really want to do is propagate that
			// uniqueness evenly across all the bits, so that we can use a subset of the bits while
			// reducing collisions significantly
			h ^= h >> 16;
			h *= 0x85ebca6b;
			h ^= h >> 13;
			h *= 0xc2b2ae35;
			return h ^ (h >> 16);
		}
	};
	template<> struct _hash_32_or_64<1> {
		static inline std::uint64_t hash(std::uint64_t h)
		{
			h ^= h >> 33;
			h *= 0xff51afd7ed558ccd;
			h ^= h >> 33;
			h *= 0xc4ceb9fe1a85ec53;
			return h ^ (h >> 33);
		}
	};
	template<std::size_t size> struct hash_32_or_64 : public _hash_32_or_64<(size > 4)> {  };

	inline size_t FNVHash(const char *pStrKey)
	{
		register size_t sizeHash = 2166136261;
		while (size_t ch = (size_t)*pStrKey++)
		{
			sizeHash *= 16777619;
			sizeHash ^= ch;
		}
		return sizeHash;
	}

	inline size_t FNVHashString(std::string& strKey)
	{
		const char *pStrKey = strKey.c_str();
		return FNVHash(pStrKey);
	}
	
	template<typename T>
	size_t HashValue(T& nKey)
	{
		return hash_32_or_64<sizeof(T)>::hash(nKey);
	}
	
	template<typename T, typename HashKEY = std::string, unsigned long dwInitSize = DEFAULT_HASH_MAP_INITSIZE>
	class CHashMap
	{
		typedef size_t(*HashCallBack)(HashKEY&);
		typedef void(*ReleaseCallBack)(T Data);
	public:
		class CVistor : public HashList::IVistor<T, HashKEY>
		{
		public:
			virtual void Vistor(std::shared_ptr<HashList::Node<T, HashKEY>>& pNode, void *pData = NULL)
			{
				m_vResultNode.push_back(pNode);
			}

			std::vector<std::shared_ptr<HashList::Node<T, HashKEY>>>& GetResult()
			{
				return m_vResultNode;
			}

		private:
			std::vector<std::shared_ptr<HashList::Node<T, HashKEY>>> m_vResultNode;
		};
		
		CHashMap()
			: m_nSize(0)
			, m_HashCallBack(NULL)
		{
			
		}

		CHashMap(HashCallBack pCallBack)
			: m_nSize(0)
			, m_HashCallBack(pCallBack)
		{
			
		}

		virtual ~CHashMap()
		{

		}

		size_t GetSize()
		{
			return m_nSize;
		}

		bool AddNode(std::shared_ptr<HashList::Node<T, HashKEY>>& pNode)
		{
			if (NULL == pNode)
			{
				return false;
			}

			unsigned long dwHashIndex = GetHashIndex(pNode->GetKey());
			if (InitHashArray(dwHashIndex)->AddNode(pNode))
			{
				m_nSize++;
				return true;
			}
			return false;
		}

		bool AddNode(HashKEY Key, T Data, ReleaseCallBack pCallBack = NULL)
		{
			std::shared_ptr<HashList::Node<T, HashKEY>> pData(new HashList::Node<T, HashKEY>(Data, pCallBack));
			pData->SetKey(Key);
			return AddNode(pData);
		}

		bool RemoveNode(std::shared_ptr<HashList::Node<T, HashKEY>>& pNode)
		{
			if (NULL == pNode)
			{
				return false;
			}

			unsigned long dwHashIndex = GetHashIndex(pNode->GetKey());
			if (InitHashArray(dwHashIndex)->RemoveNode(pNode))
			{
				m_nSize--;
				return true;
			}
			return false;
		}

		std::shared_ptr<HashList::Node<T, HashKEY>> RemoveNode()
		{
			unsigned long dwHashIndex = GetHashIndex(HashKEY());
			std::shared_ptr<HashList::Node<T, HashKEY>> pNode = InitHashArray(dwHashIndex)->RemoveNode();
			if (NULL != pNode)
			{
				m_nSize--;
				return pNode;
			}
			return std::shared_ptr<HashList::Node<T, HashKEY>>();
		}

		bool RemoveNode(HashKEY Key)
		{
			std::vector<std::shared_ptr<HashList::Node<T, HashKEY>>> vNode = GetKeyNode(Key);
			bool bResult = false;
			for (std::shared_ptr<HashList::Node<T, HashKEY>> pNode : vNode)
			{
				bResult = RemoveNode(pNode);
				if (!bResult)
				{
					break;
				}
			}
			return bResult;
		}

		bool RemoveOnlyOneNode(HashKEY Key)
		{
			std::shared_ptr<HashList::Node<T, HashKEY >> pNode = GetNodeBykey(Key);
			if (NULL == pNode)
			{
				return false;
			}

			return RemoveNode(pNode);
		}

		void FreeHashMapNode()
		{
			m_nSize = 0;

			for (unsigned long dwIndex = 0; dwIndex < dwInitSize; ++dwIndex)
			{
				if (NULL != m_arrayHashData[dwIndex])
				{
					m_arrayHashData[dwIndex]->FreeAllNode();
				}
			}
		}

		void VistorNode(HashList::IVistor<T, HashKEY>& Vistor, void *pData = NULL)
		{
			for (int nIndex = 0; nIndex < dwInitSize; ++nIndex)
			{
				std::shared_ptr<HashList::CList<T, HashKEY>> pListItem = m_arrayHashData[nIndex];
				if (NULL != pListItem)
				{
					pListItem->VistorNode(Vistor, pData);
				}
			}
		}

		void VistorKeyNode(HashList::IVistor<T, HashKEY>& Vistor, HashKEY Key, void *pData = NULL)
		{
			unsigned long dwHashIndex = GetHashIndex(Key);
			std::shared_ptr<HashList::CList<T, HashKEY>> pListItem = m_arrayHashData[dwHashIndex];
			if (NULL != pListItem)
			{
				pListItem->VistorNode(Vistor, Key, pData);
			}
		}

		std::vector<std::shared_ptr<HashList::Node<T, HashKEY>>> GetKeyNode(HashKEY Key)
		{
			CVistor Vistor;
			VistorKeyNode(Vistor, Key);
			return std::move(Vistor.GetResult());
		}
		
		std::shared_ptr<HashList::Node<T, HashKEY>> GetNodeBykey(HashKEY Key)
		{
			unsigned long dwHashIndex = GetHashIndex(Key);
			std::shared_ptr<HashList::CList<T, HashKEY>> pListItem = m_arrayHashData[dwHashIndex];
			if (NULL != pListItem)
			{
				return pListItem->GetNodeBykey(Key);
			}
			return std::shared_ptr<HashList::Node<T, HashKEY>>();
		}

		std::shared_ptr<HashList::Node<T, HashKEY>> operator[](HashKEY Key)
		{
			return GetNodeBykey(Key);
		}

	private:
		std::shared_ptr<HashList::CList<T, HashKEY>> InitHashArray(unsigned long dwHashIndex)
		{
			if (NULL == m_arrayHashData[dwHashIndex])
			{
				std::lock_guard<std::mutex> Lock(m_mutexInit);
				if (NULL == m_arrayHashData[dwHashIndex])
				{
					m_arrayHashData[dwHashIndex] = std::shared_ptr<HashList::CList<T, HashKEY>>(new HashList::CList<T, HashKEY>());
				}
			}
			return m_arrayHashData[dwHashIndex];
		}
		
		unsigned long GetThreadHashIndex()
		{
			#if defined(_WIN32) || defined(_WIN64)
				unsigned long dwThreadID = GetCurrentThreadId();
				return dwThreadID % dwInitSize;
			#else
				pid_t tThreadID = gettid();
				return tThreadID % dwInitSize;
			#endif
		}

		inline unsigned long GetHashIndex(HashKEY Key)
		{
			if (NULL == m_HashCallBack)
			{
				return GetThreadHashIndex();
			}
			else
			{
				size_t nHash = m_HashCallBack(Key);
				return nHash % dwInitSize;
			}
		}

	private:
		std::atomic<size_t> m_nSize;
		HashCallBack m_HashCallBack;
		std::mutex m_mutexInit;
		std::shared_ptr<HashList::CList<T, HashKEY>> m_arrayHashData[dwInitSize];
	};
}
/*****************************************************************************
*  @file     List.h
*  @brief    哈希表单向链表。
*  Details.                                                                 
*                                                                           
*  @author   IDeath
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-18 10:20:05
*****************************************************************************/
#pragma once
#include <string>
#include <memory>
#include "Platform.h"

namespace HashList
{
	template<typename T, typename HashKEY>
	class Node
	{
	public:
		typedef void(*ReleaseCallBack)(T Data);
		Node()
			: Data()
			, m_bLock(FALSE)
			, m_bIsRemoving(FALSE)
			, m_pCallBack(NULL)
		{
			memset(&m_HashKey, 0, sizeof(m_HashKey));
		}

		Node(T Data)
			: Data(Data)
			, m_bLock(FALSE)
			, m_bIsRemoving(FALSE)
			, m_pCallBack(NULL)
		{
			memset(&m_HashKey, 0, sizeof(m_HashKey));
		}

		Node(T Data, ReleaseCallBack CallBack)
			: Data(Data)
			, m_bLock(FALSE)
			, m_bIsRemoving(FALSE)
			, m_pCallBack(CallBack)
		{
			memset(&m_HashKey, 0, sizeof(m_HashKey));
		}

		~Node()
		{
			if (TRUE == m_bIsRemoving && NULL != m_pCallBack)
			{
				m_pCallBack(Data);
			}
		}

		std::shared_ptr<Node>& GetNext()
		{
			return m_pNext;
		}

		void SetNext(std::shared_ptr<Node>& pNext)
		{
			m_pNext = pNext;
		}

		void SetKey(HashKEY Key)
		{
			m_HashKey = Key;
		}

		T GetData()
		{
			return Data;
		}

		void SetData(T rData)
		{
			Data = rData;
		}

		HashKEY GetKey()
		{
			return m_HashKey;
		}

		void SetNodeIsRemoving()
		{
			#if defined(_WIN32) || defined(_WIN64)
				InterlockedExchange(&m_bIsRemoving, TRUE);
			#else
				__sync_bool_compare_and_swap(&m_bIsRemoving, FALSE, TRUE);
			#endif
		}

		bool IsRemoving()
		{
			return m_bIsRemoving == TRUE;
		}

		inline void SleepTime(unsigned int nTime)
		{
			#if defined(_WIN32) || defined(_WIN64)
				Sleep(nTime);
			#else
				usleep(nTime);
			#endif
		}

		void LockNode(long nLockCount = 2048)
		{
			long lWaitCount = 0;
			long nCircle = 0, nIndex = 0, nCount = 0;
			#if defined(_WIN32) || defined(_WIN64)
				// 如果*m_bLock == FALSE，那么*m_Lock设置为FALSE，并返回FALSE
				// 如果*m_bLock == TRUE，那么返回TRUE
				while (InterlockedCompareExchange(&m_bLock, TRUE, FALSE) == TRUE)
				{
					for (nCircle = 1; nCircle < nLockCount; nCircle <<= 1)
					{
						for (nIndex = 0; nIndex < nCircle; ++nIndex)
						{
							nCount++;
							/*
							__asm
							{
								pause
							}
							*/
						}
				
						if (InterlockedCompareExchange(&m_bLock, TRUE, FALSE) == FALSE)
						{
							return;
						}
					}

					if (!SwitchToThread())
					{
						SleepTime(0);
					}
				}
			#else
				while (__sync_bool_compare_and_swap(&m_bIsRemoving, FALSE, TRUE))
				{
					SleepTime(0);
					if (nLockCount == INFINITE || lWaitCount++ > nLockCount)
					{
						SleepTime(1);
						lWaitCount = 0;
					}
				}
			#endif
		}

		void UnLockNode()
		{
			#if defined(_WIN32) || defined(_WIN64)
				InterlockedExchange(&m_bLock, FALSE);
			#else
				__sync_bool_compare_and_swap(&m_bIsRemoving, TRUE, FALSE);
			#endif
		}

	private:
		std::shared_ptr<Node> m_pNext;
		HashKEY m_HashKey;
		T Data;
		volatile long m_bLock;
		volatile long m_bIsRemoving;
		ReleaseCallBack m_pCallBack;
		Node(const Node& Other) = delete;
		Node& operator=(const Node& Other) = delete;
	};

	template<typename T, typename HashKEY>
	class CLockHelper
	{
	public:
		CLockHelper(std::shared_ptr<Node<T, HashKEY>>& pNode)
			: m_pNode(pNode)
		{
			if (NULL != m_pNode)
			{
				m_pNode->LockNode();
			}
		}

		~CLockHelper()
		{
			if (NULL != m_pNode)
			{
				m_pNode->UnLockNode();
			}
		}

	private:
		std::shared_ptr<Node<T, HashKEY>> m_pNode;
	};

	template<typename T, typename HashKEY>
	class IVistor
	{
	public:

		virtual void Vistor(std::shared_ptr<Node<T, HashKEY>>& pNode, void *pData = NULL) {};
	};

	template<typename T, typename HashKEY>
	class CList
	{
	public:
		CList()
		{
			m_pFNode = std::make_shared<Node<T, HashKEY>>();
		}

		~CList()
		{
			FreeAllNode();
			m_pFNode = NULL;
		}

		bool AddNode(std::shared_ptr<Node<T, HashKEY>>& pNode)
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			do
			{
				CLockHelper<T, HashKEY> HelperP(pPrvNode);
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					pPrvNode->SetNext(pNode);
				}
				else
				{
					CLockHelper<T, HashKEY> HelperN(pNextNode);
					pNode->SetNext(pNextNode);
					pPrvNode->SetNext(pNode);
				}
			} while (false);
			return true;
		}

		bool RemoveNode(std::shared_ptr<Node<T, HashKEY>>& pNode)
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			CLockHelper<T, HashKEY> LockFNode(m_pFNode);
			bool bResult = false;
			do
			{
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					break;
				}

				CLockHelper<T, HashKEY> HelperN(pNextNode);
				if (pNextNode == pNode)
				{
					pNode->SetNodeIsRemoving();
					pPrvNode->SetNext(pNextNode->GetNext());
					bResult = true;
					break;
				}

				pPrvNode = pNextNode;
			} while (true);
			return bResult;
		}

		std::shared_ptr<Node<T, HashKEY>> RemoveNode()
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			CLockHelper<T, HashKEY> LockFNode(m_pFNode);
			std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
			if (NULL == pNextNode)
			{
				return std::shared_ptr<Node<T, HashKEY>>();
			}

			pPrvNode->SetNext(pNextNode->GetNext());
			return pNextNode;
		}

		void VistorNode(IVistor<T, HashKEY>& Vistor, void *pData = NULL)
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			do
			{
				CLockHelper<T, HashKEY> HelperP(pPrvNode);
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					break;
				}

				CLockHelper<T, HashKEY> HelperN(pNextNode);
				if (!pNextNode->IsRemoving())
				{
					Vistor.Vistor(pNextNode, pData);
				}

				pPrvNode = pNextNode;
			} while (true);
		}

		void VistorNode(IVistor<T, HashKEY>& Vistor, HashKEY Key, void *pData = NULL)
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			do
			{
				CLockHelper<T, HashKEY> HelperP(pPrvNode);
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					break;
				}

				CLockHelper<T, HashKEY> HelperN(pNextNode);
				if (pNextNode->GetKey() == Key && !pNextNode->IsRemoving())
				{
					Vistor.Vistor(pNextNode, pData);
				}

				pPrvNode = pNextNode;
			} while (true);
		}

		std::shared_ptr<Node<T, HashKEY>> GetNodeBykey(HashKEY Key)
		{
			std::shared_ptr<Node<T, HashKEY>> pPrvNode = m_pFNode;
			std::shared_ptr<Node<T, HashKEY>> pResult;
			do
			{
				CLockHelper<T, HashKEY> HelperP(pPrvNode);
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					break;
				}

				CLockHelper<T, HashKEY> HelperN(pNextNode);
				if (pNextNode->GetKey() == Key && !pNextNode->IsRemoving())
				{
					pResult = pNextNode;
					break;
				}

				pPrvNode = pNextNode;
			} while (true);
			return pResult;
		}
		
		void FreeAllNode()
		{
			std::shared_ptr<Node<T, HashKEY>> pNextNode;
			std::shared_ptr<Node<T, HashKEY>> pEmpty;
			{
				CLockHelper<T, HashKEY> LockFNode(m_pFNode);
				pNextNode = m_pFNode->GetNext();
				m_pFNode->SetNext(pEmpty);
			}

			std::shared_ptr<Node<T, HashKEY>> pPrvNode = pNextNode;
			do
			{
				if (NULL == pPrvNode)
				{
					break;
				}

				CLockHelper<T, HashKEY> HelperP(pPrvNode);
				pPrvNode->SetNodeIsRemoving();
				std::shared_ptr<Node<T, HashKEY>> pNextNode = pPrvNode->GetNext();
				if (NULL == pNextNode)
				{
					break;
				}
				
				pPrvNode->SetNext(pEmpty);
				pPrvNode = pNextNode;
			} while (true);
		}

	private:
		std::shared_ptr<Node<T, HashKEY>> m_pFNode;
	};
}
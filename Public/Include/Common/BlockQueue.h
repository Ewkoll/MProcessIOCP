/*****************************************************************************
*  @file     BlockQueue.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2013-02-04 15:04:21
*****************************************************************************/
#ifndef BlockQueue_h__
#define BlockQueue_h__

#include <list>
#include <condition_variable>

template<typename T> class BlockingQueue
{
public:
	typedef void(*ReleaseCallBack)(T Data);
	BlockingQueue()
		: m_pCallBack(NULL)
		, m_bInterrupt(false)
	{

	}
	
	BlockingQueue(ReleaseCallBack pCallBcak)
		: m_bInterrupt(false)
		, m_pCallBack(pCallBcak)
	{

	}
	
	~BlockingQueue()
	{
		if (NULL != m_pCallBack)
		{
			for (auto& it : m_ListQuene)
			{
				m_pCallBack(it);
			}
		}
		m_ListQuene.clear();
	}

	void Put(const T& Data)
	{
		std::unique_lock<std::mutex> Lock(m_Mutex);
		m_ListQuene.push_back(Data);
		Lock.unlock();
		m_ConditionVariable.notify_one();
	}

	T Take()
	{
		std::unique_lock<std::mutex> Lock(m_Mutex);
		m_ConditionVariable.wait(Lock, [this] { return m_bInterrupt || !m_ListQuene.empty(); });
		if (m_bInterrupt)
		{
			return T();
		}

		T Front(m_ListQuene.front());
		m_ListQuene.pop_front();
		return Front;
	}

	size_t Size() const
	{
		std::unique_lock<std::mutex> Lock(m_Mutex);
		return m_ListQuene.size();
	}

	void Interrupt()
	{
		m_bInterrupt = true;
		m_ConditionVariable.notify_all();
	}

private:
	BlockingQueue(const BlockingQueue& rOther) = delete;
	BlockingQueue& operator=(const BlockingQueue& rOther) = delete;

private:
	mutable std::mutex m_Mutex;
	ReleaseCallBack m_pCallBack;
	bool m_bInterrupt;
	std::list<T> m_ListQuene;
	std::condition_variable m_ConditionVariable;	
};

#endif // BlockQueue_h__
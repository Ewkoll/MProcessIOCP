/*****************************************************************************
*  @file     Sington.h
*  @brief    单件
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/20 17:25:09
*****************************************************************************/
#ifndef Sington_h__
#define Sington_h__

#pragma once
#include <mutex>
#include <list>
#include <algorithm>
#include <functional>

/**
* @brief 自动清理，负责对单件的实现调用清理函数等同于OnExit();
*/
class CAutoClearManager
{
public:
	CAutoClearManager()
	{

	}

	virtual ~CAutoClearManager()
	{
		for (std::function<int()> ClearFunction : m_ListSingleton)
		{
			ClearFunction();
		}
		m_ListSingleton.clear();
	}

	static CAutoClearManager& GetInstance()
	{
		static CAutoClearManager Instance;
		return Instance;
	}

	void RegisterClearInstacne(std::function<int()> pCallBack)
	{
		m_ListSingleton.push_back(pCallBack);
	};

private:

	std::list<std::function<int()>> m_ListSingleton;	///< 保存单件释放映射。
};

/**
* @brief 单件模板类，只需继承即可。
*/
template<class T> class Singleton
{
public:
	Singleton()
	{

	}

	virtual ~Singleton()
	{

	}

	static T& GetInstance()
	{
		if (nullptr == gInstance)
		{
			std::lock_guard<std::mutex> LockInstace(g_Mutex);
			if (nullptr == gInstance)
			{
				gInstance = new T;
				CAutoClearManager::GetInstance().RegisterClearInstacne(std::bind(Singleton::DeleteInstance));
			}
		}
		return *gInstance;
	}

	static int DeleteInstance()
	{
		if (nullptr != gInstance)
		{
			std::lock_guard<std::mutex> LockInstace(g_Mutex);
			if (nullptr != gInstance)
			{
				delete gInstance;
				gInstance = nullptr;
			}
		}
		return 0;
	}

private:
	static T* volatile gInstance;	///< 单件实列
	static std::mutex g_Mutex;		///< 锁。
};

template<class T> T* volatile Singleton<T>::gInstance = nullptr;
template<class T> std::mutex Singleton<T>::g_Mutex;

#define SingletonClass(Name, ParentCalssName) \
class Name : public ParentCalssName, public Singleton<Name> {};

#endif // Sington_h__
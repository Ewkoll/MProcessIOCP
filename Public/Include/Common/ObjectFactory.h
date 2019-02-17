/*****************************************************************************
*  @file     ObjectFactory.h
*  @brief    对象工厂模板类。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/04/28 14:37:16
*****************************************************************************/
#ifndef ObjectFactory_h__
#define ObjectFactory_h__

#pragma once
#include "IObject.h"

namespace Dooms
{
	class CObjectFactory
	{
	public:
		/**
		* @brief 映射类型。
		*/
		typedef std::map<std::string, std::function<IObject*()>> MAPType;

		/**
		* @brief 创建对象模板函数。
		*/
		template<class T, class ...Args>
		static std::shared_ptr<T> CreateObjectT(Args&&... _Args)
		{
			return std::make_shared<T>(std::forward<Args>(_Args)...);
		}

		/**
		* @brief 类名称注册。
		*/
		template<class T>
		static void RegisterClassFactory()
		{
			mapConstructors().insert(std::make_pair(typeid(T).name(), std::bind(&ConstructorHelper<T>)));
		}

		/**
		* @brief UUID注册。
		*/
		template<class T>
		static void RegisterClassFactory(std::string strUUID)
		{
			if (strUUID.empty())
			{
				return;
			}
			mapConstructors().insert(std::make_pair(strUUID, std::bind(&ConstructorHelper<T>)));
		}
		
		/**
		* @brief 创建对象。
		*/
		template<class T>
		static std::shared_ptr<T> CreateObject()
		{
			MAPType::iterator it = mapConstructors().find(typeid(T).name());
			if (it != mapConstructors().end())
			{
				return std::shared_ptr<T>(dynamic_cast<T*>((it->second)()));
			}
			return std::shared_ptr<T>();
		}

		/**
		* @brief 根据类型名称创建对象。
		*/
		template<class T>
		static std::shared_ptr<T> CreateObject(std::string strUUID)
		{
			MAPType::iterator it = mapConstructors().find(strUUID);
			if (it != mapConstructors().end())
			{
				return std::shared_ptr<T>(dynamic_cast<T*>((it->second)()));
			}
			return std::shared_ptr<T>();
		}

		/**
		* @brief 获取所有支持的组件名称。
		*/
		static std::vector<std::string> GetClassObject()
		{
			std::vector<std::string> vResult;
			MAPType& mapType = mapConstructors();
			for (auto it : mapType)
			{
				vResult.push_back(it.first);
			}
			return vResult;
		}
		
	protected:
		/**
		* @brief 构造模板函数类。
		*/
		template<class T>
		static T* ConstructorHelper()
		{
			return new T();
		}

		/**
		* @brief 构造MAP映射。
		*/
		static MAPType& mapConstructors()
		{
			static MAPType Instance;
			return Instance;
		}
	};

	#define REGISTER_OBJECT_CLASS(Name, UUID_NAME)	\
	CObjectFactory::RegisterClassFactory<Name>();	\
	CObjectFactory::RegisterClassFactory<Name>(UUID_NAME);
}

#endif // ObjectFactory_h__
/*****************************************************************************
*  @file     Dir.h
*  @brief    目录枚举。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/21 13:22:22
*****************************************************************************/
#ifndef Dir_h__
#define Dir_h__

#pragma once
#include <string>
#include <list>
#include "Common/Global.h"

/**
* @brief 获取对应目录下的相关文件。
*/
class DLL_EXPORT CDir
{
public:
	class DLL_EXPORT CDirVistor
	{
	public:
		/**
		* @brief 目录信息访问器
		*/
		virtual void Vistor(bool bDir, const std::string& strFile, const std::string& strPath, const std::string& strExtension) = 0;
	};
	
	/**
	* @brief: 	  获取目录下的所有文件，并返回List。
	* @param [in] strPath
	* @return     std::list<std::string>
	*/
	std::list<std::string> EnumFilePath(const std::string& strPath);

	/**
	* @brief: 	  获取目录下的所有文件，并通过访问器返回。
	* @param [in] strPath
	* @param [in] Vistor
	* @return     void
	*/
	void EnumFilePath(const std::string& strPath, CDirVistor& Vistor);

	/**
	* @brief: 	  递归获取目录下的所有文件【包含子目录】访问器返回。
	* @param [in] bRecursive
	* @param [in] strPath
	* @param [in] Vistor
	* @return     void
	*/
	void EnumFilePath(bool bRecursive, const std::string& strPath, CDirVistor& Vistor);
};

#endif // Dir_h__
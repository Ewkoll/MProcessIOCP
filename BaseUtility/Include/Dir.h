/*****************************************************************************
*  @file     Dir.h
*  @brief    Ŀ¼ö�١�
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
* @brief ��ȡ��ӦĿ¼�µ�����ļ���
*/
class DLL_EXPORT CDir
{
public:
	class DLL_EXPORT CDirVistor
	{
	public:
		/**
		* @brief Ŀ¼��Ϣ������
		*/
		virtual void Vistor(bool bDir, const std::string& strFile, const std::string& strPath, const std::string& strExtension) = 0;
	};
	
	/**
	* @brief: 	  ��ȡĿ¼�µ������ļ���������List��
	* @param [in] strPath
	* @return     std::list<std::string>
	*/
	std::list<std::string> EnumFilePath(const std::string& strPath);

	/**
	* @brief: 	  ��ȡĿ¼�µ������ļ�����ͨ�����������ء�
	* @param [in] strPath
	* @param [in] Vistor
	* @return     void
	*/
	void EnumFilePath(const std::string& strPath, CDirVistor& Vistor);

	/**
	* @brief: 	  �ݹ��ȡĿ¼�µ������ļ���������Ŀ¼�����������ء�
	* @param [in] bRecursive
	* @param [in] strPath
	* @param [in] Vistor
	* @return     void
	*/
	void EnumFilePath(bool bRecursive, const std::string& strPath, CDirVistor& Vistor);
};

#endif // Dir_h__
/*****************************************************************************
*  @file     WorkPath.h
*  @brief    �洢��ǰ�Ĺ���Ŀ¼�����ṩ�Զ��ָ���
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/21 10:18:08
*****************************************************************************/
#ifndef WorkPath_h__
#define WorkPath_h__

#pragma once
#include <string>
#include "Common/Global.h"

class CWorkPathIMPL;

/**
* @brief DLL���ص�ʱ�򣬿�����Ҫ�л�����Ŀ¼��
*/
class DLL_EXPORT CWorkPath
{
public:

	CWorkPath();

	CWorkPath(const std::string& strPath);

	virtual ~CWorkPath();

	bool SetWorkPath(const std::string& strPath);
	
	std::string GetModulePath();

	std::string GetCurWorkPath();

private:
	
	CWorkPathIMPL *m_pIMPL;
};

#endif // WorkPath_h__
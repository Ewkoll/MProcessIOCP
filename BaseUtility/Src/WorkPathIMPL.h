/*****************************************************************************
*  @file     WorkPathIMPL.h
*  @brief    �洢��ǰ�Ĺ���Ŀ¼�����ṩ�Զ��ָ���
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/21 10:18:08
*****************************************************************************/
#ifndef WorkPathIMPL_h__
#define WorkPathIMPL_h__

#pragma once
#include <string>

/**
* @brief DLL���ص�ʱ�򣬿�����Ҫ�л�����Ŀ¼��
*/
class CWorkPathIMPL
{
public:

	CWorkPathIMPL();

	CWorkPathIMPL(const std::string& strPath);

	virtual ~CWorkPathIMPL();

	bool SetWorkPath(const std::string& strPath);

	std::string GetModulePath();

	std::string GetCurWorkPath();

private:
	std::string m_strSrcWorkPath;	///< ԭ������Ŀ¼��
	std::string m_strCurWorkPath;	///< ��ǰ����Ŀ¼��
};

#endif // WorkPathIMPL_h__
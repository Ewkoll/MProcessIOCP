/*****************************************************************************
*  @file     WorkPathIMPL.h
*  @brief    存储当前的工作目录。并提供自动恢复。
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
* @brief DLL加载的时候，可能需要切换工作目录。
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
	std::string m_strSrcWorkPath;	///< 原来工作目录。
	std::string m_strCurWorkPath;	///< 当前工作目录。
};

#endif // WorkPathIMPL_h__
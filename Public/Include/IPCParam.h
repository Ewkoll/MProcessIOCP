/*****************************************************************************
*  @file     IPCParam.h
*  @brief    IPC基本参数。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-31 10:52:11
*****************************************************************************/
#ifndef IPCParam_h__
#define IPCParam_h__

#pragma once
#include <string>
#include <wtypes.h>
#include <algorithm>
#include <WinSock2.h>

class IPCParam
{
public:
	IPCParam()
	{
		m_hAcceptEvent = WSA_INVALID_EVENT;
	}

	unsigned int GetMasterID()
	{
		return m_nMasterID;
	}

	void SetMasterID(unsigned int nMasterID)
	{
		m_nMasterID = nMasterID;
	}

	std::string GetMasterPath()
	{
		return m_strMasterPath;
	}

	void SetMasterPath(std::string strMasterPath)
	{
		m_strMasterPath = strMasterPath;
	}

	std::string GetMasterName()
	{
		return m_strMasterName;
	}

	void SetMasterName(std::string strMasterName)
	{
		m_strMasterName = strMasterName;
	}

	void SetMasterNameByFullPath(std::string strFullPath)
	{
		size_t nPos = (std::max)(strFullPath.rfind('/'), strFullPath.rfind('\\'));
		if (nPos != -1)
		{
			m_strMasterPath = strFullPath.substr(0, nPos);
			m_strMasterName = strFullPath.substr(nPos + 1, strFullPath.length() - nPos);
		}
	}

	int GetPort()
	{
		return m_nPort;
	}

	void SetPort(int nPort)
	{
		m_nPort = nPort;
	}

	std::string GetIP()
	{
		return m_strIP;
	}

	void SetIP(std::string strIP)
	{
		m_strIP = strIP;
	}
	
	HANDLE GetAcceptEvent()
	{
		return m_hAcceptEvent;
	}

	void SetAcceptEvent(HANDLE hEvent)
	{
		m_hAcceptEvent = hEvent;
	}

private:
	unsigned int m_nMasterID;			///< 主进程PID
	std::string m_strMasterPath;		///< 主进程路径
	std::string m_strMasterName;		///< 主进程名称
	HANDLE m_hAcceptEvent;				///< 等待事件
	int m_nPort;						///＜监听端口
	std::string m_strIP;				///< 监听地址
};

#endif // IPCParam_h__
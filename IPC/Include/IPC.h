/*****************************************************************************
*  @file     IPC.h
*  @brief    IPC组件
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-28 14:30:05
*****************************************************************************/
#ifndef IPC_h__
#define IPC_h__

#pragma once
#include <sstream>
#include <typeinfo>
#include <wtypes.h>
#include "Common\Global.h"

enum E_IPC_MODE
{
	E_IPC_MODE_SENDER,
	E_IPC_MODE_BOTH,
	E_IPC_MODE_RECEIVER,
};

enum E_IPC_TYPE
{
	E_IPC_TYPE_MESSAGEQUENE
};

class IPC
{
public:
	
	virtual bool InitIPC(const char* pIpcName, E_IPC_MODE nMode, size_t nMaxMsg = 0, size_t nMaxMsgSize = 0) = 0;

	virtual bool UnInitIPC(const char* pIpcName) = 0;

	virtual bool Send(void* pBuffer, size_t nLength) = 0;

	virtual bool TimeSend(void* pBuffer, size_t nLength, size_t nMillisecond) = 0;

	virtual bool Receive(void* pBuffer, size_t nLength) = 0;

	virtual bool TimeReceive(void* pBuffer, size_t nLength, size_t nMillisecond) = 0;
};

template<typename T, E_IPC_TYPE nType = E_IPC_TYPE_MESSAGEQUENE>
class IPCModule
{
public:
	IPCModule(DWORD dwMasterID, DWORD dwWorkerID, std::string& strIPCName, bool bUnit = false, E_IPC_MODE ipcMode = E_IPC_MODE_BOTH, size_t nMaxMsg = 1024)
		: m_pIPCLayer(NULL)
		, m_bInitResult(false)
		, m_nMode(ipcMode)
	{
		m_pIPCLayer = GetIPCLayer(nType);
		do 
		{
			if (NULL == m_pIPCLayer)
			{
				break;
			}
			
			std::stringstream StringStream;
			StringStream << strIPCName << "-" << typeid(T).name() << "-" << dwMasterID << "-" << dwWorkerID;
			m_strName = StringStream.str();
			const char *pIPCName = m_strName.c_str();
			if (bUnit)
			{
				if (!m_pIPCLayer->UnInitIPC(pIPCName))
				{
					break;
				}
			}
			
			if (!(m_bInitResult = m_pIPCLayer->InitIPC(pIPCName, ipcMode, nMaxMsg, sizeof(T))))
			{
				break;
			}
		} while (false);
	};
	
	virtual ~IPCModule()
	{
		FreeIPCModule();
	}

	bool ReInitIPC(size_t nMaxMsg = 1024)
	{
		if (NULL == m_pIPCLayer)
		{
			return false;
		}
		
		m_bInitResult = m_pIPCLayer->InitIPC(m_strName.c_str(), m_nMode, nMaxMsg, sizeof(T));
		return m_bInitResult;
	}

	void UnInitIPC(std::string& strIPCName)
	{
		if (NULL != m_pIPCLayer)
		{
			m_pIPCLayer->UnInitIPC(strIPCName.c_str());
		}
	}

	void FreeIPCModule()
	{
		if (NULL != m_pIPCLayer)
		{
			FreeIPCLayer(m_pIPCLayer);
			m_pIPCLayer = NULL;
		}
		m_bInitResult = false;
	}

	bool GetStatus() 
	{
		return m_bInitResult; 
	};

	bool Send(void* pBuffer, size_t nLength)
	{
		if (NULL == m_pIPCLayer || false == m_bInitResult)
		{
			return false;
		}

		return m_pIPCLayer->Send(pBuffer, nLength);
	}

	bool TimeSend(void* pBuffer, size_t nLength, size_t nMillisecond)
	{
		if (NULL == m_pIPCLayer || false == m_bInitResult)
		{
			return false;
		}

		return m_pIPCLayer->TimeSend(pBuffer, nLength, nMillisecond);
	}

	bool Receive(void* pBuffer, size_t nLength)
	{
		if (NULL == m_pIPCLayer || false == m_bInitResult)
		{
			return false;
		}

		return m_pIPCLayer->Receive(pBuffer, nLength);
	}

	bool TimeReceive(void* pBuffer, size_t nLength, size_t nMillisecond)
	{
		if (NULL == m_pIPCLayer || false == m_bInitResult)
		{
			return false;
		}

		return m_pIPCLayer->TimeReceive(pBuffer, nLength, nMillisecond);
	}

private:
	IPC *m_pIPCLayer;			///< IPC层。
	bool m_bInitResult;			///< 初始化状态。
	E_IPC_MODE m_nMode;			///< 模式。
	std::string m_strName;		///< IPC名称。
};

#ifdef __cplusplus
extern "C"
{
#endif
	DLL_EXPORT IPC* GetIPCLayer(E_IPC_TYPE nType);
	DLL_EXPORT void FreeIPCLayer(IPC* pIPC);
#ifdef __cplusplus
}
#endif

#endif // IPC_h__
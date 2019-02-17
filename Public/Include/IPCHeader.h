/*****************************************************************************
*  @file     IPCHeader.h
*  @brief    IPC 数据协议， 消息队列不支持动态大小包。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-07-28 17:32:55
*****************************************************************************/
#pragma once
#include <vector>
#include <atomic>
#include <wtypes.h>

#define MASTER_IPCNAME	std::string("157FD604-0F53-4907-BC73-D546BD2CE434-MASTER_PREFIX_IPCNAME")
#define WORKER_IPCNAME	std::string("A372A868-C4AB-4A79-8D48-A30450065651-WORKER_PREFIX_IPCNAME")
#define IPC_TIMEOUT 3000

enum E_IPC_PACKET_TYPE
{
	E_IPC_PACKET_TYPE_NULL = 0x0,
	E_IPC_PACKET_TYPE_PING,
	E_IPC_PACKET_TYPE_PONG,
	E_IPC_PACKET_TYPE_EXIT_COMMAND,
	E_IPC_PACKET_TYPE_EXIT_RESULT,

	E_IPC_PACKET_TYPE_BUSINESS = 0x1000,
	E_IPC_PACKET_TYPE_WSASOCKET_COMMAND,
	E_IPC_PACKET_TYPE_WSASOCKET_RESULT,
};

enum E_IPC_CODE
{
	E_IPC_CODE_EXIT_SUCCESS = 10000,
	E_IPC_CODE_EXIT_FAILED = -1,
};

class CResult
{
public:
	CResult(E_IPC_CODE nCode, std::string strResultMSG)
	{
		m_nCode = nCode;
		strncpy_s(m_szResultMSG, strResultMSG.length(), strResultMSG.c_str(), sizeof(m_szResultMSG) - 1);
	}
	CResult()
	{
		m_nCode = E_IPC_CODE_EXIT_SUCCESS;
		memset(m_szResultMSG, 0, sizeof(m_szResultMSG));
	}
	E_IPC_CODE m_nCode;					///< 返回错误。
	char m_szResultMSG[256];			///< 错误信息。
};

class CIPCData
{
public:
	template<typename T> void SetData(E_IPC_PACKET_TYPE nType, T& tData)
	{
		m_nPacketType = nType;
		memcpy(m_bBuffer, &tData, sizeof(tData));
	};
	template<typename T> T& GetData()
	{
		T *pData = (T*)(m_bBuffer);
		return *pData;
	};
	CIPCData()
	{
		m_nPacketType = E_IPC_PACKET_TYPE_NULL;
		memset(m_bBuffer, 0, sizeof(m_bBuffer));
	}
	CIPCData(E_IPC_PACKET_TYPE nType)
	{
		m_nPacketType = nType;
		memset(m_bBuffer, 0, sizeof(m_bBuffer));
	}
	CIPCData(const CIPCData& Other)
	{
		*this = Other;
	}
	CIPCData& operator=(const CIPCData& Other)
	{
		if (this == &Other)
		{
			return *this;
		}

		m_nPacketType = Other.m_nPacketType;
		memcpy(m_bBuffer, Other.m_bBuffer, sizeof(m_bBuffer));
		return *this;
	}
	E_IPC_PACKET_TYPE m_nPacketType;	///< IPC包类型。
	byte m_bBuffer[1024];				///< 不支持大于1024字节的数据通信。
};
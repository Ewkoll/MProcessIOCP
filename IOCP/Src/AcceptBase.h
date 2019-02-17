/*****************************************************************************
*  @file     AcceptBase.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                 
*  @email    IDeath@operatorworld.com                             
*  @version  0.0.0.1														
*  @date	 19/01/28 15:55:11
*****************************************************************************/
#ifndef AcceptBase_h__
#define AcceptBase_h__

#pragma once
#include <memory>
#include <functional>
#include "Platform.h"
#include "Worker.h"
#include "HashMap/HashMap.h"

typedef std::function<bool(SOCKET&)> AssociateSocket;

class CAcceptBase : public CWorker
{
public:
	CAcceptBase(std::string strName = std::string())
		: CWorker(false, strName)
		, m_bAcceptConnections(true)
		, m_hListenSocket(INVALID_SOCKET)
	{

	}

	virtual void SetListenSocket(SOCKET hListenSocket)
	{
		m_hListenSocket = hListenSocket;
	}

	virtual void RejectClientConnect(SOCKET hSocket)
	{
		if (hSocket == INVALID_SOCKET)
		{
			return;
		}

		LINGER LingerStruct = LINGER{ 1, 0 };
		setsockopt(hSocket, SOL_SOCKET, SO_LINGER, (char *)&LingerStruct, sizeof(LingerStruct));

		CancelIo((HANDLE)hSocket);

		closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}

	virtual void AddBannedIP(SOCKADDR_IN& SockAddr)
	{
		m_BanIPHashMAP.AddNode(SockAddr.sin_addr.S_un.S_addr, SockAddr.sin_addr.S_un.S_addr);
	}

	virtual bool IsInBannedList(SOCKADDR_IN* pCaller)
	{
		if (NULL == pCaller)
		{
			return false;
		}

		std::vector<std::shared_ptr<HashList::Node<ULONG, ULONG>>> vSize = m_BanIPHashMAP.GetKeyNode(pCaller->sin_addr.S_un.S_addr);
		return !vSize.empty();
	}

	virtual void RemoveBannedIP(SOCKADDR_IN& SockAddr)
	{
		m_BanIPHashMAP.RemoveNode(SockAddr.sin_addr.S_un.S_addr);
	}

	virtual void SetAcceptConnections(bool bAcceptConnections)
	{
		m_bAcceptConnections = bAcceptConnections;
	}

	virtual void SetAssociateSocketCallBack(AssociateSocket pCallBack)
	{
		m_pFuncAssociateIncomingClientSocket = pCallBack;
	}

protected:
	SOCKET m_hListenSocket;									///< 监听套接字。
	HashMAP::CHashMap<ULONG, ULONG, 24> m_BanIPHashMAP;		///< 黑名单哈希表。
	volatile bool m_bAcceptConnections;						///< 是否接受连接。
	AssociateSocket m_pFuncAssociateIncomingClientSocket;	///< 设置接受连接后的处理。
};

static int CALLBACK ConnectAcceptCondition(
	IN LPWSABUF lpCallerId,
	IN LPWSABUF lpCallerData,
	IN OUT LPQOS lpSQOS,
	IN OUT LPQOS lpGQOS,
	IN LPWSABUF lpCalleeId,
	IN LPWSABUF lpCalleeData,
	OUT GROUP FAR * g,
	IN DWORD_PTR dwCallbackData)
{
	CAcceptBase* pThis = reinterpret_cast<CAcceptBase*>(dwCallbackData);
	if (NULL == pThis || NULL != pThis && pThis->IsInBannedList((SOCKADDR_IN*)lpCallerId->buf))
	{
		return CF_REJECT;
	}
	return CF_ACCEPT;
}

#endif // AcceptBase_h__
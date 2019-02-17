/*****************************************************************************
*  @file     ConnectionCheck.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                 
*  @email    IDeath@operatorworld.com                             
*  @version  0.0.0.1														
*  @date	 19/01/18 15:18:05
*****************************************************************************/
#ifndef ConnectionCheck_h__
#define ConnectionCheck_h__
#pragma once

#include <memory>
#include "Worker.h"
#include "HashMap/List.h"
#include "ClientManager.h"

class CConnectionCheck : public CWorker
					   , public HashList::IVistor<std::shared_ptr<IOCP::CClientContext>, SOCKET>
{
public:

	CConnectionCheck(bool bBlock, std::string strName = std::string());

	virtual ~CConnectionCheck();

	virtual bool Start() override;

	virtual bool Run() override;

	virtual bool Exit() override;

	virtual void Vistor(std::shared_ptr<Node<std::shared_ptr<IOCP::CClientContext>, SOCKET>>& pNode, void *pData = NULL) override;
	
private:

	volatile bool m_bRunning;								///< ÔËÐÐ×´Ì¬¡£
};

#endif // ConnectionCheck_h__
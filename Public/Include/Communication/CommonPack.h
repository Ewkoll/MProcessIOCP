/*****************************************************************************
*  @file     CommonPack.h
*  @brief    通用包头。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2019-01-11 14:31:27
*****************************************************************************/
#ifndef CommonPack_h__
#define CommonPack_h__

#include "Communication\IOPack.h"

typedef struct _PACKET_
{
	unsigned char Magic[4];			// 标记头。
	int			nType;				// 类型。
	int			nLength;			// 报文大小。
	int			nCheckSum;			// 校验码。
} PACKET, *PPACKET;

class CBusinessPacketBody
{
public:
	CBusinessPacketBody()
		: m_nBodySize(0)
		, m_pBody(std::shared_ptr<char>())
	{
		memset(&m_Header, 0, sizeof(PACKET));
	}

	CBusinessPacketBody(PACKET& Header)
		: m_Header(Header)
		, m_nBodySize(0)
		, m_pBody(std::shared_ptr<char>())
	{
		
	}

	CBusinessPacketBody(PACKET& Header, unsigned int nBodySize, std::shared_ptr<char>& pBody)
		: m_Header(Header)
		, m_nBodySize(nBodySize)
		, m_pBody(pBody)
	{

	}
	
public:
	PACKET m_Header;
	unsigned int m_nBodySize = 0;
	std::shared_ptr<char> m_pBody;
};

class CCommonPack
{
public:

	bool UnPackHeader(PACKET& Header, CIOPack& rPack);

	bool UnPackBody(PACKET& Header, CIOPack& rPack, std::shared_ptr<char>& pBody, int& nSize);

	bool PackData(CIOPack& IOPack, PACKET& Header, void *pData, int nLength);
};

#endif // CommonPack_h__
#include "Communication\IOPack.h"
#include <winsock.h>

#define MAGIC_STRING_CODE	0xAD3243FE
#define MAGIC_MEMORY_CODE	0xCADE1DCE

CIOPack::CIOPack(int nLength /* = 512 */)
	: m_nPos(0)
	, m_nUse(0)
	, m_nLength(nLength)
{
	if (nLength <= sizeof(m_szBuffer))
	{
		m_nLength = sizeof(m_szBuffer);
	}

	if (m_nLength != sizeof(m_szBuffer))
	{
		GenerateNewBuffer(m_nLength);
	}
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
}

CIOPack::CIOPack(void* pData, int nLength)
	: m_nPos(0)
	, m_nUse(nLength)
	, m_nLength(nLength)
{
	if (NULL == pData || 0 == nLength)
	{
		throw PackException(std::string("param exception!"));
		return;
	}
	
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	if (nLength <= sizeof(m_szBuffer))
	{
		memcpy(m_szBuffer, pData, nLength);
	}
	else
	{
		GenerateNewBuffer(m_nLength);
		memcpy(m_pBuffer.get(), pData, nLength);
	}
}

CIOPack::CIOPack(const CIOPack& Other)
{
	*this = Other;
}

CIOPack& CIOPack::operator=(const CIOPack& Other)
{
	if (this == &Other)
	{
		return *this;
	}

	m_nLength = Other.m_nLength;
	m_nPos = Other.m_nPos;
	m_nUse = Other.m_nUse;
	memcpy(m_szBuffer, Other.m_szBuffer, sizeof(m_szBuffer));
	if (NULL != Other.m_pBuffer)
	{
		m_pBuffer = std::shared_ptr<unsigned char>(new unsigned char[m_nLength], std::default_delete<unsigned char[]>());
		memcpy(m_pBuffer.get(), Other.m_pBuffer.get(), m_nLength);
	}
	return *this;
}

void CIOPack::Reset()
{
	m_nPos = 0;
}

CIOPack& CIOPack::operator<<(unsigned char cData)
{
	return InternelAddData(&cData, sizeof(cData));
}

CIOPack& CIOPack::operator<<(int nData)
{
	int nNetData = htonl(nData);
	return InternelAddData((char*)&nNetData, sizeof(nNetData));
}

CIOPack& CIOPack::operator<<(unsigned int nData)
{
	unsigned int nNetData = htonl(nData);
	return InternelAddData((char*)&nNetData, sizeof(nNetData));
}

CIOPack& CIOPack::operator<<(unsigned short nData)
{
	unsigned int nNetData = htons(nData);
	return InternelAddData((char*)&nNetData, sizeof(nNetData));
}

CIOPack& CIOPack::operator<<(float nData)
{
	return InternelAddData((char*)&nData, sizeof(nData));
}

CIOPack& CIOPack::operator<<(double nData)
{
	return InternelAddData((char*)&nData, sizeof(nData));
}

CIOPack& CIOPack::operator<<(char* pData)
{
	operator<<(MAGIC_STRING_CODE);
	operator<<((int)strlen(pData));
	return InternelAddData(pData, (int)strlen(pData));
}

CIOPack& CIOPack::operator<<(const std::string& strData)
{
	operator<<(MAGIC_STRING_CODE);
	operator<<((int)strData.length());
	return InternelAddData((void*)strData.c_str(), (int)strData.length());
}

CIOPack& CIOPack::AddData(char *pData, int nSize)
{
	operator<<(MAGIC_MEMORY_CODE);
	operator<<(nSize);
	return InternelAddData(pData, nSize);
}

CIOPack& CIOPack::AddArrayData(void *pData, int nSize)
{
	return InternelAddData((char*)pData, nSize);
}

CIOPack& CIOPack::operator>>(unsigned char& cData)
{
	if (m_nPos + (int)sizeof(cData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	cData = *pData;
	m_nPos += sizeof(cData);
	return *this;
}

CIOPack& CIOPack::operator>>(int& nData)
{
	if (m_nPos + (int)sizeof(nData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	nData = ntohl(*(int*)pData);
	m_nPos += sizeof(nData);
	return *this;
}

CIOPack& CIOPack::operator>>(unsigned int& nData)
{
	if (m_nPos + (int)sizeof(nData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	nData = ntohl(*(unsigned int*)pData);
	m_nPos += sizeof(nData);
	return *this;
}

CIOPack& CIOPack::operator>>(unsigned short& nData)
{
	if (m_nPos + (int)sizeof(nData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	nData = ntohs(*(unsigned short*)pData);
	m_nPos += sizeof(nData);
	return *this;
}

CIOPack& CIOPack::operator>>(float& nData)
{
	if (m_nPos + (int)sizeof(nData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	memcpy(&nData, pData, sizeof(nData));
	m_nPos += sizeof(nData);
	return *this;
}

CIOPack& CIOPack::operator>>(double& nData)
{
	if (m_nPos + (int)sizeof(nData) > m_nUse)
	{
		throw PackException(std::string("parse package exception!"));
	}

	unsigned char *pData = GetBuffer() + m_nPos;
	memcpy(&nData, pData, sizeof(nData));
	m_nPos += sizeof(nData);
	return *this;
}

CIOPack& CIOPack::operator>>(char* pData)
{
	if (NULL == pData)
	{
		throw PackException(std::string("param exception!"));
	}

	unsigned int nMagicCode = 0;
	operator>>(nMagicCode);
	if (nMagicCode != MAGIC_STRING_CODE)
	{
		throw PackException(std::string("parse package exception!"));
	}

	int nLength = 0;
	operator>>(nLength);
	if (m_nPos + nLength > m_nLength)
	{
		throw PackException(std::string("parse package exception!"));
	}

	memcpy(pData, GetBuffer() + m_nPos, nLength);
	m_nPos += nLength;
	return *this;
}

CIOPack& CIOPack::operator>>(std::shared_ptr<char>& pData)
{
	if (NULL == pData)
	{
		throw PackException(std::string("param exception!"));
	}

	unsigned int nMagicCode = 0;
	operator>>(nMagicCode);
	if (nMagicCode != MAGIC_STRING_CODE)
	{
		throw PackException(std::string("parse package exception!"));
	}

	int nLength = 0;
	operator>>(nLength);
	if (m_nPos + nLength > m_nLength)
	{
		throw PackException(std::string("parse package exception!"));
	}

	pData = std::shared_ptr<char>(new char[static_cast<unsigned __int64>(nLength) + 1], std::default_delete<char[]>());
	memcpy(pData.get(), GetBuffer() + m_nPos, nLength);
	pData.get()[nLength] = '\x0';
	m_nPos += nLength;
	return *this;
}

CIOPack& CIOPack::operator>>(std::string& strData)
{
	std::shared_ptr<char> pData;
	operator>>(pData);
	strData = pData.get();
	return *this;
}

CIOPack& CIOPack::ParseData(std::shared_ptr<char>& pData, int& nSize)
{
	unsigned int nMagicCode = 0;
	operator>>(nMagicCode);
	if (nMagicCode != MAGIC_MEMORY_CODE)
	{
		throw PackException(std::string("parse package exception!"));
	}

	operator>>(nSize);
	if (m_nPos + nSize > m_nLength)
	{
		throw PackException(std::string("parse package exception!"));
	}

	pData = std::shared_ptr<char>(new char[static_cast<unsigned __int64>(nSize) + 1], std::default_delete<char[]>());
	memcpy(pData.get(), GetBuffer() + m_nPos, nSize);
	pData.get()[nSize] = '\x0';
	m_nPos += nSize;
	return *this;
}

CIOPack& CIOPack::ParseArrayData(char *pData, int nSize)
{
	if (NULL == pData || m_nPos + nSize > m_nLength)
	{
		throw PackException(std::string("parse package exception!"));
	}

	memcpy(pData, GetBuffer() + m_nPos, nSize);
	m_nPos += nSize;
	return *this;
}

CIOPack& CIOPack::InternelAddData(void *pData, int nSize)
{
	if ((m_nUse + nSize) > m_nLength)
	{
		GenerateNewBuffer((m_nLength + nSize) * 2);
	}

	unsigned char *pPos = (GetBuffer() + m_nUse);
	memcpy(pPos, pData, nSize);
	m_nUse += nSize;
	return *this;
}

void CIOPack::GenerateNewBuffer(int nLength)
{
	if (nLength > 1024 * 1024 * 1024)
	{
		throw PackException(std::string("package length exception!"));
	}

	std::shared_ptr<unsigned char> pBuffer = std::shared_ptr<unsigned char>(new unsigned char[nLength], std::default_delete<unsigned char[]>());
	if (NULL == pBuffer)
	{
		throw PackException(std::string("alloc memory exception!"));
	}

	memset(pBuffer.get(), 0, nLength * sizeof(char));
	if (NULL != m_pBuffer)
	{
		memmove(pBuffer.get(), m_pBuffer.get(), m_nLength);
	}
	else
	{
		memmove(pBuffer.get(), m_szBuffer, sizeof(m_szBuffer));
	}
	m_pBuffer = pBuffer;
	m_nLength = nLength;
}

unsigned char* CIOPack::GetBuffer()
{
	if (m_nLength <= sizeof(m_szBuffer))
	{
		return m_szBuffer;
	}
	else
	{
		if (NULL == m_pBuffer)
		{
			throw PackException(std::string("unintended exception!"));
		}
		return m_pBuffer.get();
	}
}
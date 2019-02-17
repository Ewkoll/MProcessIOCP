#include "IOCPBuffer.h"
#include "DebugLog.h"
#include <algorithm>

using namespace IOCP;

CIOCPBuffer::CIOCPBuffer(CIOCPBufferManager& rBufferManager, unsigned int nDataLength/* = IOBUFFER_DATA_LENGTH*/)
	: m_rBufferManager(rBufferManager)
	, m_nUsed(0)
	, m_nOperationType(OP_NULL)
	, m_nDataLength(IOBUFFER_DATA_LENGTH)
{
	Init(nDataLength);
}

CIOCPBuffer::~CIOCPBuffer()
{

}

void CIOCPBuffer::Init(unsigned int nDataLength /*= IOBUFFER_DATA_LENGTH*/)
{
	if (nDataLength < IOBUFFER_DATA_LENGTH)
	{
		nDataLength = IOBUFFER_DATA_LENGTH;
	}

	if (nDataLength > m_nDataLength)
	{
		m_pDataBuffer = std::shared_ptr<char>(new char[nDataLength], std::default_delete<char[]>());
		memset(m_pDataBuffer.get(), 0, nDataLength);
	}

	m_nOperationType = OP_NULL;
	m_nDataLength = (std::max)((int)nDataLength, (int)m_nDataLength);
	m_nUsed = 0;
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	memset(&m_wsaDataBuf, 0, sizeof(m_wsaDataBuf));
}

void CIOCPBuffer::SetPoolClient(IExecutor* pPoolClient)
{
	m_pPoolClient = pPoolClient;
}

IExecutor* CIOCPBuffer::GetPoolClient()
{
	return m_pPoolClient;
}

WSABUF* CIOCPBuffer::GetWSABuffer()
{
	return const_cast<WSABUF*>(&m_wsaDataBuf);
}

bool CIOCPBuffer::AddData(char *pData, unsigned int nLength)
{
	if (NULL == pData || 0 == nLength || INVALID_LENGTH == nLength)
	{
		return false;
	}

	if (nLength > m_nDataLength - m_nUsed)
	{
		if (!GenerateNewBuffer(GetNewLength(nLength + m_nUsed)))
		{
			return false;
		}
	}
	
	memcpy(GetBuffer() + m_nUsed, pData, nLength);
	m_nUsed += nLength;
	return true;
}

bool CIOCPBuffer::SetData(char *pData, unsigned int nLength)
{
	if (NULL == pData || 0 == nLength || INVALID_LENGTH == nLength)
	{
		return false;
	}

	if (nLength > m_nDataLength)
	{
		if (!GenerateNewBuffer(GetNewLength(nLength)))
		{
			return false;
		}
	}
	
	memcpy(GetBuffer(), pData, nLength);
	m_nUsed = nLength;
	return true;
}

bool CIOCPBuffer::Flush(unsigned int nBytesToRemove)
{
	if (0 == nBytesToRemove)
	{
		return true;
	}

	if ((nBytesToRemove > m_nDataLength) || (nBytesToRemove > m_nUsed))
	{
		ErrorFileLog("移除不使用的内存越界[m_nUsed = %d][m_nDataLength=%d][nBytesToRemove = %d]", m_nUsed, m_nDataLength, nBytesToRemove);
		return false;
	}
	
	m_nUsed -= nBytesToRemove;
	memmove(GetBuffer(), GetBuffer() + nBytesToRemove, m_nUsed);
	return true;
}

bool IOCP::CIOCPBuffer::SetupWrite(int nWriteLenth /*= INVALID_LENGTH*/)
{
	if (INVALID_LENGTH == nWriteLenth)
	{
		m_wsaDataBuf.buf = reinterpret_cast<char*>(GetBuffer());
		m_wsaDataBuf.len = m_nUsed;
	}
	else
	{
		if (m_nUsed - nWriteLenth < 0)
		{
			return false;
		}

		m_wsaDataBuf.buf = reinterpret_cast<char*>(GetBuffer() + nWriteLenth);
		m_wsaDataBuf.len = m_nUsed - nWriteLenth;
	}
	return true;
}

bool CIOCPBuffer::SetupRead(int nReadLenth /*= INVALID_LENGTH*/)
{
	int nValidLength = m_nDataLength - m_nUsed;
	if (nValidLength == 0 || (nReadLenth != INVALID_LENGTH && nReadLenth > nValidLength))
	{
		int nNewLength = nReadLenth == INVALID_LENGTH ? nNewLength = GetNewLength(m_nDataLength) : nNewLength = GetNewLength(m_nUsed + nReadLenth);
		if (!GenerateNewBuffer(nNewLength))
		{
			return false;
		}
	}

	m_wsaDataBuf.buf = reinterpret_cast<char*>(GetBuffer() + m_nUsed);
	if (nReadLenth != INVALID_LENGTH)
	{
		m_wsaDataBuf.len = nReadLenth;
	}
	else
	{
		m_wsaDataBuf.len = m_nDataLength - m_nUsed;
	}
	return true;
}

void CIOCPBuffer::SetupZeroByteRead()
{
	m_wsaDataBuf.buf = reinterpret_cast<char*>(GetBuffer());
	m_wsaDataBuf.len = 0;
}

OPERATION_TYPE CIOCPBuffer::GetOperation()
{
	return (OPERATION_TYPE)m_nOperationType;
}

void CIOCPBuffer::SetOperation(unsigned int nOperationType)
{
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	m_nOperationType = nOperationType;
}

void CIOCPBuffer::EmptyUsed()
{
	m_nUsed = 0;
}

unsigned int CIOCPBuffer::Use(unsigned int nSize)
{
	m_nUsed += nSize;
	return m_nUsed;
}

unsigned int CIOCPBuffer::GetUsed()
{
	return m_nUsed;
}

CIOCPBufferManager& CIOCPBuffer::GetBufferManager()
{
	return m_rBufferManager;
}

char* CIOCPBuffer::GetBuffer()
{
	if (m_nDataLength <= IOBUFFER_DATA_LENGTH)
	{
		return m_szBuffer;
	}
	else
	{
		return m_pDataBuffer.get();
	}
}

int CIOCPBuffer::GetNewLength(int nNewLength)
{
	if (nNewLength < IOBUFFER_DATA_LENGTH * 10)
	{
		return nNewLength * 2;
	}
	else
	{
		return int(nNewLength * 1.5);
	}
}

bool CIOCPBuffer::GenerateNewBuffer(int nLength)
{
	if (nLength > 1024 * 1024 * 1024)
	{
		return false;
	}

	std::shared_ptr<char> pBuffer = std::shared_ptr<char>(new char[nLength], std::default_delete<char[]>());
	memset(pBuffer.get(), 0, nLength);
	if (m_nDataLength == IOBUFFER_DATA_LENGTH)
	{
		memmove(pBuffer.get(), m_szBuffer, m_nUsed);
		memset(m_szBuffer, 0, sizeof(m_szBuffer));
		m_pDataBuffer = pBuffer;
		m_nDataLength = nLength;
	}
	else
	{
		memmove(pBuffer.get(), m_pDataBuffer.get(), m_nUsed);
		m_pDataBuffer = pBuffer;
		m_nDataLength = nLength;
	}
	return true;
}
/*****************************************************************************
*  @file     IOPack.h
*  @brief    组包，数组和字符串无法区分增加AddArrayData函数。
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-08 12:10:12
*****************************************************************************/
#ifndef IOPack_h__
#define IOPack_h__

#pragma once
#include <memory>
#include <string>

class PackException : public std::exception
{
public:
	PackException(const std::string& strException)
		: std::exception(strException.c_str())
	{

	}
};

class CIOPack
{
public:
	CIOPack(int nLength = 512);
	CIOPack(void* pData, int nLength);
	CIOPack(const CIOPack& Other);
	CIOPack& operator=(const CIOPack& Other);
	CIOPack& operator==(const CIOPack& Other) = delete;
	int GetUseSize() { return m_nUse; };
	void OffestUse(int nUse) { m_nUse += nUse; };
	void EmptyUse() { m_nUse = 0; };
	void Reset();
	int GetPos() { return m_nPos; };
	void SkipReadPos(int nPos) { m_nPos += nPos; };
	int GetRemainderSize() { return m_nUse - m_nPos; };
	char* GetData() { return (char*)GetBuffer(); };
	unsigned int GetLength() { return m_nLength; };
	unsigned int GetUseLength() { return m_nUse; };

	CIOPack& operator<<(unsigned char cData);
	CIOPack& operator<<(int nData);
	CIOPack& operator<<(unsigned int nData);
	CIOPack& operator<<(unsigned short nData);
	CIOPack& operator<<(float nData);
	CIOPack& operator<<(double nData);
	CIOPack& operator<<(char* pData);
	CIOPack& operator<<(const std::string& strData);
	CIOPack& AddData(char *pData, int nSize);
	CIOPack& AddArrayData(void *pData, int nSize);
	
	CIOPack& operator>>(unsigned char& cData);
	CIOPack& operator>>(int& nData);
	CIOPack& operator>>(unsigned int& nData);
	CIOPack& operator>>(unsigned short& nData);
	CIOPack& operator>>(float& nData);
	CIOPack& operator>>(double& nData);
	CIOPack& operator>>(char* pData);
	CIOPack& operator>>(std::shared_ptr<char>& pData);
	CIOPack& operator>>(std::string& strData);
	CIOPack& ParseData(std::shared_ptr<char>& pData, int& nSize);
	CIOPack& ParseArrayData(char *pData, int nSize);

private:
	void GenerateNewBuffer(int nLength);
	inline unsigned char* GetBuffer();
	CIOPack& InternelAddData(void *pData, int nSize);

private:
	int m_nLength;								///< 总缓存长度。
	int m_nPos;									///< 读取偏移。
	int m_nUse;									///< 写入偏移。
	unsigned char m_szBuffer[512];				///< 栈存储。
	std::shared_ptr<unsigned char> m_pBuffer;	///< 大于512时候。
};

#endif // IOPack_h__
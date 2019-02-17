/*****************************************************************************
*  @file     IOCPBuffer.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath    
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1														
*  @date	 2017-08-03 15:05:13
*****************************************************************************/
#pragma once
#include <atomic>
#include "IOPool.h"

namespace IOCP
{
	enum OPERATION_TYPE
	{
		OP_NULL,						///< 刚刚初始化的类型。
		OP_IOInitialize,				///< 客户端刚连接。
		OP_IORead,						///< 读取数据从客户端。
		OP_IOReadCompleted,				///< 读取数据完成。
		OP_IOWrite,						///< 写入数据到客户端。
		OP_IOWriteCompleted,			///< 写入数据完成。
	};

	#define INVALID_LENGTH -1
	#define IOBUFFER_DATA_LENGTH 1024

	class CIOCPBufferManager;
	class CIOCPBuffer : public OVERLAPPEDEX
	{
	public:

		CIOCPBuffer(CIOCPBufferManager& rBufferManager, unsigned int nDataLength = IOBUFFER_DATA_LENGTH);

		virtual ~CIOCPBuffer();
		
		void Init(unsigned int nDataLength = IOBUFFER_DATA_LENGTH);

		void SetPoolClient(IExecutor* pPoolClient);

		IExecutor* GetPoolClient();

		WSABUF* GetWSABuffer();
		
		bool AddData(char *pData, unsigned int nLength);

		bool SetData(char *pData, unsigned int nLength);

		bool Flush(unsigned int nBytesToRemove);
		
		bool SetupWrite(int nWriteLenth = INVALID_LENGTH);

		bool SetupRead(int nReadLenth = INVALID_LENGTH);

		void SetupZeroByteRead();

		OPERATION_TYPE GetOperation();

		void SetOperation(unsigned int nOperationType);

		void EmptyUsed();

		unsigned int Use(unsigned int nSize);

		unsigned int GetUsed();

		inline char* GetBuffer();

		CIOCPBufferManager& GetBufferManager();

	private:

		inline int GetNewLength(int nNewLength);

		bool GenerateNewBuffer(int nLength);
	
	private:
		static std::atomic<int> m_nCount;			///< BUFFER数量。
		CIOCPBufferManager& m_rBufferManager;		///< 缓冲区管理。
		WSABUF m_wsaDataBuf;						///< WSABUF。
		unsigned int m_nDataLength;					///< 总共缓存区长度。
		unsigned int m_nUsed;						///< 使用的空间。
		unsigned int m_nOperationType;				///< BUFFER类型。
		std::shared_ptr<char> m_pDataBuffer;		///< 缓存内存。
		char m_szBuffer[IOBUFFER_DATA_LENGTH];		///< 小缓存。
	};
}
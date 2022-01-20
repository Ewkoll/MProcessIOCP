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
		OP_NULL,						///< �ոճ�ʼ�������͡�
		OP_IOInitialize,				///< �ͻ��˸����ӡ�
		OP_IORead,						///< ��ȡ���ݴӿͻ��ˡ�
		OP_IOReadCompleted,				///< ��ȡ������ɡ�
		OP_IOWrite,						///< д�����ݵ��ͻ��ˡ�
		OP_IOWriteCompleted,			///< д��������ɡ�
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
		static std::atomic<int> m_nCount;			///< BUFFER������
		CIOCPBufferManager& m_rBufferManager;		///< ����������
		WSABUF m_wsaDataBuf;						///< WSABUF��
		unsigned int m_nDataLength;					///< �ܹ����������ȡ�
		unsigned int m_nUsed;						///< ʹ�õĿռ䡣
		unsigned int m_nOperationType;				///< BUFFER���͡�
		std::shared_ptr<char> m_pDataBuffer;		///< �����ڴ档
		char m_szBuffer[IOBUFFER_DATA_LENGTH];		///< С���档
	};
}
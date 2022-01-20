#include "CommonProtocol.h"
#include "AsyncOperator.h"
#include "CommunicationWorker.h"
#include "Communication\IOPack.h"
#include "Communication\CommonPack.h"

using namespace IOCP;

CCommonProtocol::CCommonProtocol()
{

}

CCommonProtocol::~CCommonProtocol()
{

}

void CCommonProtocol::SetServerExecuteCallBack(ServerExecute pCallBack)
{
	m_pCallBack = pCallBack;
}

bool CCommonProtocol::ParseReceivePackage(CIOCPBuffer *pIOCPBuffer, DWORD dwSize, std::shared_ptr<CClientContext>& pContext, ReceivePackageCallBack& pCallBack)
{
	#define MAX_SINGLE_PACKET_SIZE 64 * 1024 * 1024
	E_RECEIVE_PKG_STATUS nStatus = E_RECEIVE_PKG_FINISHED;
	bool bResult = false;
	pContext->CheckWriteQueueStatus();
	IExecutor* pExecutor = pIOCPBuffer->GetPoolClient();
	std::list<CBusinessPacketBody> ListRecvPacket;
	CIOPack IOPack(pIOCPBuffer->GetBuffer(), pIOCPBuffer->GetUsed());
	do 
	{
		// ��ͷ���ȶ�û�н��յ��������������ݡ�
		if (IOPack.GetRemainderSize() < sizeof(PACKET))
		{
			bResult = true;
			nStatus = E_RECEIVE_PKG_UNFINISHED;
			break;
		}

		PACKET Header;
		// ����ͷ����
		if (!m_CommonPack.UnPackHeader(Header, IOPack))
		{
			bResult = false;
			break;
		}

		// ��ͷ���ݳ��ȴ���64M����Ϊ���쳣����
		if (Header.nLength >= MAX_SINGLE_PACKET_SIZE)
		{
			bResult = false;
			break;
		}

		// ��ͷ��û�н�����ɣ������������ݡ�
		if (IOPack.GetRemainderSize() < Header.nLength)
		{
			IOPack.SkipReadPos(-1 * (int)sizeof(PACKET));
			bResult = true;
			nStatus = E_RECEIVE_PKG_UNFINISHED;
			break;
		}

		CBusinessPacketBody bPacket(Header);
		// �������塣
		if (!m_CommonPack.UnPackBody(Header, IOPack, bPacket.m_pBody, (int&)bPacket.m_nBodySize))
		{
			bResult = false;
			break;
		}

		ListRecvPacket.push_back(bPacket);
		// �������ʣ��ķְ����ݣ�������ȡ��֡�
		if (IOPack.GetRemainderSize() <= 0)
		{
			bResult = true;
			break;
		}
	} while (true);
	
	// ˢ���Ѿ�����Ļ�������
	bResult = bResult && pIOCPBuffer->Flush(IOPack.GetPos());
	
	// �ص�����Ͷ�ݽ���
	bResult = bResult && pCallBack(nStatus, pIOCPBuffer, pContext);

	// ִ��Ĭ�ϲ�����
	if (NULL == m_pCallBack)
	{
		if (ListRecvPacket.size() > 0)
		{
			for (std::list<CBusinessPacketBody>::iterator it = ListRecvPacket.begin(); it != ListRecvPacket.end(); ++it)
			{
				IOPack.EmptyUse();
				if (m_CommonPack.PackData(IOPack, it->m_Header, it->m_pBody.get(), it->m_nBodySize))
				{
					if (!CAsyncOperator::AsyncSend(pContext, IOPack.GetData(), IOPack.GetUseLength(), pExecutor))
					{
						DebugConsoleLog("�첽�ط�����Ͷ��ʧ�� --- �յ���������[%d][%s][%d]", it->m_Header.nLength, it->m_pBody.get(), it->m_nBodySize);
						bResult = false;
						break;
					}
				}
			}
		}
		return bResult;
	}
	else
	{
		return bResult && m_pCallBack(ListRecvPacket, pContext, pExecutor);
	}
}
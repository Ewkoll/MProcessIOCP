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
		// 包头长度都没有接收到，继续接收数据。
		if (IOPack.GetRemainderSize() < sizeof(PACKET))
		{
			bResult = true;
			nStatus = E_RECEIVE_PKG_UNFINISHED;
			break;
		}

		PACKET Header;
		// 解析头部。
		if (!m_CommonPack.UnPackHeader(Header, IOPack))
		{
			bResult = false;
			break;
		}

		// 包头数据长度大于64M，认为是异常包。
		if (Header.nLength >= MAX_SINGLE_PACKET_SIZE)
		{
			bResult = false;
			break;
		}

		// 包头还没有接收完成，继续接收数据。
		if (IOPack.GetRemainderSize() < Header.nLength)
		{
			IOPack.SkipReadPos(-1 * (int)sizeof(PACKET));
			bResult = true;
			nStatus = E_RECEIVE_PKG_UNFINISHED;
			break;
		}

		CBusinessPacketBody bPacket(Header);
		// 解析包体。
		if (!m_CommonPack.UnPackBody(Header, IOPack, bPacket.m_pBody, (int&)bPacket.m_nBodySize))
		{
			bResult = false;
			break;
		}

		ListRecvPacket.push_back(bPacket);
		// 如果还有剩余的分包数据，继续读取拆分。
		if (IOPack.GetRemainderSize() <= 0)
		{
			bResult = true;
			break;
		}
	} while (true);
	
	// 刷新已经处理的缓冲区。
	bResult = bResult && pIOCPBuffer->Flush(IOPack.GetPos());
	
	// 回调先行投递接收
	bResult = bResult && pCallBack(nStatus, pIOCPBuffer, pContext);

	// 执行默认操作。
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
						DebugConsoleLog("异步回发数据投递失败 --- 收到包体数据[%d][%s][%d]", it->m_Header.nLength, it->m_pBody.get(), it->m_nBodySize);
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
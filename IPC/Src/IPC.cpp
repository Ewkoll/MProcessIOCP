#include "IPC.h"
#include "MessageQuene.h"

IPC* GetIPCLayer(E_IPC_TYPE nType)
{
	if (nType == E_IPC_TYPE_MESSAGEQUENE)
	{
		return new CMessageQuene();
	}
	return NULL;
}

void FreeIPCLayer(IPC* pIPC)
{
	if (NULL == pIPC)
	{
		return;
	}
	delete pIPC;
}
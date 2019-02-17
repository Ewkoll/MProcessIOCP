#include "DebugLog.h"
#include "Communication/CommonPack.h"

bool CCommonPack::UnPackHeader(PACKET& Header, CIOPack& rPack)
{
	bool bResult = false;
	do
	{
		try
		{
			rPack.ParseArrayData((char*)Header.Magic, sizeof(Header.Magic));
			rPack >> Header.nType >> Header.nLength >> Header.nCheckSum;
			if (Header.Magic[0] != 0xFA || Header.Magic[1] != 0xCA || Header.Magic[2] != 0x12 || Header.Magic[3] != 0x13)
			{
				break;
			}

			bResult = true;
		}
		catch (PackException& e) { DebugFileLog(e.what()); }
	} while (false);
	return bResult;
}

bool CCommonPack::UnPackBody(PACKET& Header, CIOPack& rPack, std::shared_ptr<char>& pBody, int& nSize)
{
	bool bResult = false;
	do
	{
		try
		{
			rPack.ParseData(pBody, nSize);
			bResult = true;
		}
		catch (PackException& e) { DebugFileLog(e.what()); }
	} while (false);
	return bResult;
}

bool CCommonPack::PackData(CIOPack& IOPack, PACKET& Header, void *pData, int nLength)
{
	bool bResult = false;
	do
	{
		try
		{
			IOPack.AddArrayData(Header.Magic, sizeof(Header.Magic));
			IOPack << Header.nType << Header.nLength << Header.nCheckSum;
			IOPack.AddData((char*)pData, nLength);
			bResult = true;
		}
		catch (PackException& e) { DebugFileLog(e.what()); }
	} while (false);
	return bResult;
}

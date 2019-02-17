#include "SystemINFO.h"
#include <Windows.h>

DWORD CSystemINFO::GetNumberOfProcessors()
{
	SYSTEM_INFO SI;
	GetSystemInfo(&SI);
	return SI.dwNumberOfProcessors;
}
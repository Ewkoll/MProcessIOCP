/*****************************************************************************
*  @file     Platform.h
*  @brief    
*  Details.                                                                 
*                                                                           
*  @author   IDeath
*  @email    IDeath@operatorworld.com
*  @version  0.0.0.1
*  @date	 2017/03/14 14:13:09
*****************************************************************************/
#ifndef Platform_h__
#define Platform_h__

#ifdef _WIN32

#include "InitSocket.h"
#include <SDKDDKVer.h>
#include <Windows.h>

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#define SLEEP(N) Sleep(N)

#else

#include <sys/types.h>
#include <sys/syscall.h>

#define gettid() syscall(__NR_gettid)

#define SLEEP(N) usleep(N * 1000)

#ifndef FALSE
#	define FALSE 0
#endif

#ifndef TRUE
#	define TRUE 1
#endif

#ifndef INFINITE
#	define INFINITE 0xFFFFFFFF
#endif

#ifndef BYTE
#	define BYTE unsigned char
#endif

#endif // _WIN32

#endif // Platform_h__
/*****************************************************************************
*  @file     Global.h
*  @brief    DLL 模块到处定义。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/18 14:26:54
*****************************************************************************/
#ifndef Global_h__
#define Global_h__
#pragma once

#ifndef STATIC_LIB
# if defined(DYNAMIC_LIB)
#	define DLL_EXPORT __declspec(dllexport)
# else
#	define DLL_EXPORT __declspec(dllimport)
# endif
#else
#	define DLL_EXPORT
#endif

#ifndef NOVTABLE
	#define NOVTABLE __declspec(novtable)
#endif

#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <functional>

#ifdef _WIN32
	#include <Windows.h>
	#include <tchar.h>
	#define LITTLE_ENDIAN
	#undef  WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	typedef HMODULE LIB_MODULE;
#endif

#define INVALID_OBJECTID 0

namespace Dooms
{	
#undef  ERROR_LENGTH
#define ERROR_LENGTH -1

#undef  ERROR
#define ERROR -1

#ifndef MAX_PATH
#	define MAX_PATH 260
#endif

#ifndef INFINITE
#	define INFINITE -1
#endif

#define MEMCMP(s1,s2,len)			memcmp(s1,s2,(len))
#define STRSTR(dest,sub)			strstr(dest,sub)
#define STRCHR(dest,ch)				strchr(dest,ch)
#define STRRCHR(dest,ch)			strrchr(dest,ch)
#define STRCMP(s1,s2)				strcmp(s1,s2)
#define STRLEN(s)					strlen(s)
#define ZEROMEMORY(dest,len)		memset((dest),0,(len))

#ifndef COUNTOF
#	define COUNTOF(x)				(sizeof(x) / sizeof(x[0]))
#endif

#ifndef SAFESTRNCPY
#	define SAFESTRNCPY(x, y)		strncpy(x, y, sizeof(x)); x[sizeof(x) -1] = 0x00;
#endif

#ifdef _WIN32

#define ITOA(value,dest,dest_sz,radix)					_itoa_s(value,dest,dest_sz,radix)                         
#define LTOA(value,dest,dest_sz,radix)					_ltoa_s(value,dest,dest_sz,radix)  	                                                                   
#define I64TOA(value,dest,dest_sz,radix)				_i64toa_s(value,dest,dest_sz,radix)

#define STRNCMP(s1,s2,count)							strncmp(s1,s2,count)
#define STRICMP(s1,s2)									_stricmp(s1,s2)
#define STRNICMP(s1,s2,count)							_strnicmp(s1,s2,count)

#define STRCAT(dest,dest_sz,source)						strcat_s(dest,dest_sz,source)
#define STRCPY(dest,dest_sz,src)						strcpy_s(dest,dest_sz,src)
#define STRNCPY(dest,dest_sz,src,count)					strncpy_s(dest,dest_sz,src,count)

#define SPRINTF(dest,dest_sz,format,...)				sprintf_s(dest,format,__VA_ARGS__)
#define SNPRINTF(dest,dest_sz,count,format,...)			snprintf(dest,(count)+1,format,__VA_ARGS__)
#define VSPRINTF(dest,dest_sz,format,argptr)			vsprintf(dest,format,argptr)
#define VSNPRINTF(dest,dest_sz,count,format,argptr)		vsnprintf(dest,count,format,argptr)

#else

#define ITOA(value,dest,dest_sz,radix)					sprintf(dest,"%d",value)                           
#define LTOA(value,dest,dest_sz,radix)					sprintf(dest,"%d",value)      	                                                                   
#define I64TOA(value,dest,dest_sz,radix)				sprintf(dest,"%lld",value)       

#define STRNCMP(s1,s2,count)							strncmp(s1,s2,count)
#define STRICMP(s1,s2)									strcasecmp(s1,s2)
#define STRNICMP(s1,s2,count)							strncasecmp(s1,s2,count)

#define STRCAT(dest,dest_sz,src)						strcat(dest,src)
#define STRCPY(dest,dest_sz,src)						strcpy(dest,src)
#define STRNCPY(dest,dest_sz,src,count)					strncpy(dest,src,count)

#define SPRINTF(dest,dest_sz,format,...)				sprintf(dest,format,##__VA_ARGS__)
#define SNPRINTF(dest,dest_sz,count,format,...)			snprintf(dest,(count)+1,format,##__VA_ARGS__)
#define VSPRINTF(dest,dest_sz,format,argptr)			vsprintf(dest,format,argptr)
#define VSNPRINTF(dest,dest_sz,count,format,argptr)		vsnprintf(dest,count,format,argptr)

#endif

#define INTERFACE_UUID(ClassName, UUID) const std::string UUID_##ClassName = UUID
}

#define CREATE_PLUGIN_FUNCNAME "CreatePlugin"
#define DESTROY_PLUGIN_FUNCNAME "DestroyPlugin"

#endif // Global_h__
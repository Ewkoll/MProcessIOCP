/* --------------------------------------------------------------------------------------------*/
//	文件名		:	ErrorLog.h
//	目的		:	日记输出类。
//	创建时间	:	2014-04-10 18:36
//	创建者		:	IDeath
//
//	历史记录	:	
/* --------------------------------------------------------------------------------------------*/
#pragma once
#ifndef ERROR_LOG_H
#define ERROR_LOG_H	

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef UNICODE
	#define DebugPrint				DebugPrintW
	#define DebugPrintLastError		DebugPrintLastErrorW
#else
	#define	DebugPrint				DebugPrintA
	#define DebugPrintLastError		DebugPrintLastErrorA
#endif

#if (_DEBUG || ERROR_LOG)
	#define DebugPrintA				ErrorLog::DbgPrintA
	#define DebugPrintLastErrorA	ErrorLog::PrintLastErrorA
#else
	#define DebugPrintA 
	#define DebugPrintLastErrorA
#endif

#if (_DEBUG || ERROR_LOG)
	#define DebugPrintW				ErrorLog::DbgPrintW
	#define DebugPrintLastErrorW	ErrorLog::PrintLastErrorW
#else
	#define DebugPrintW
	#define DebugPrintLastErrorW
#endif

#if !_AFX

#ifdef _DEBUG  
#	define ASSERT(x) {if(!(x)) _asm{int 0x03}}  
#	define VERIFY(x) {if(!(x)) _asm{int 0x03}}     // 译注：为调试版本时产生中断有效  
#else  
#define ASSERT(x)  
#	define VERIFY(x) x                             // 译注：为发行版本时不产生中断  
#endif  

#endif

#ifndef FORCE_ANSI
#define FORCE_ANSI      0x10000
#endif
#ifndef FORCE_UNICODE
#define FORCE_UNICODE   0x20000
#endif
#ifndef FORCE_INT64
#define FORCE_INT64     0x40000
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef _countof
#	define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif

//////////////////////////////////////////////////////////////////////////
namespace ErrorLog
{
	static TCHAR s_szOutputFileName[MAX_PATH];
#if _MSC_VER <= 1200
	inline int _vscprintf(const char* lpszFormat, va_list argList);
	inline int _vscwprintf(const wchar_t* lpszFormat, va_list argList);
	inline int _scprintf(const char* lpszFormat, ...);
	inline int _scwprintf(const wchar_t* lpszFormat, ...);
#endif
	inline void Trim(LPSTR lpStr, LPCSTR lpTrim);
	inline void OutputLogFile(LPCSTR lpLogInfo);
	static int DbgPrintA(const char* Format, ... );
	static int DbgPrintW(const wchar_t* Format, ...);
	static void PrintLastErrorA(const char* lpPrefix);
	static void PrintLastErrorW(const wchar_t* lpPrefix);
}

//////////////////////////////////////////////////////////////////////////
#if _MSC_VER <= 1200
#include <malloc.h>
#include <stdlib.h>

inline int ErrorLog::_vscprintf(const char* lpszFormat, va_list argList)
{
	int nMaxLen = 0;
	for (const char* lpsz = lpszFormat; *lpsz != '\0'; lpsz++)
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(++lpsz) == '%')
		{
			nMaxLen += 1;
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz++)
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++)
				;
		}
		ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz++;

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz++;
			}
			else
			{
				nPrecision = atoi(lpsz);
				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++)
					;
			}
			ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(lpsz, "I64", 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			ASSERT(FALSE);
#endif
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz++;
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz++;
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz++;
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, wchar_t);
			break;

		// strings
		case 's':
			{
				const char*pstrNextArg = va_arg(argList, const char *);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
				wchar_t *pstrNextArg = va_arg(argList, wchar_t *);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
				const char* pstrNextArg = va_arg(argList, const char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
				wchar_t* pstrNextArg = va_arg(argList, wchar_t*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				{
					double f;
					char* pszTemp;

					// 312 == strlen("-1+(309 zeroes).")
					// 309 zeroes == max precision of a double
					// 6 == adjustment in case precision is not specified,
					//   which means that the precision defaults to 6
					pszTemp = (char*)_alloca(max(nWidth, 312+nPrecision+6));

					f = va_arg(argList, double);
					sprintf( pszTemp, "%*.*f", nWidth, nPrecision+6, f );
					nItemLen = strlen(pszTemp);
				}
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				ASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}
	return nMaxLen;
}

inline int ErrorLog::_vscwprintf(const wchar_t* lpszFormat, va_list argList)
{
	int nMaxLen = 0;
	for (const wchar_t* lpsz = lpszFormat; *lpsz != '\0'; lpsz++)
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz++) == '%')
		{
			nMaxLen += 1;
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz++)
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _wtoi(lpsz);
			for (; *lpsz != '\0' && iswdigit(*lpsz); lpsz++)
				;
		}
		ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz++;

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz++;
			}
			else
			{
				nPrecision = _wtoi(lpsz);
				for (; *lpsz != '\0' && iswdigit(*lpsz); lpsz++)
					;
			}
			ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (wcsncmp(lpsz, L"I64", 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			ASSERT(FALSE);
#endif
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz++;
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz++;
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz++;
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, wchar_t);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, wchar_t);
			break;

		// strings
		case 's':
			{
				const wchar_t*pstrNextArg = va_arg(argList, const wchar_t*);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
				const char* pstrNextArg = va_arg(argList, const char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
				wchar_t* pstrNextArg = va_arg(argList, wchar_t*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				{
					double f;
					wchar_t* pszTemp;

					// 312 == strlen("-1+(309 zeroes).")
					// 309 zeroes == max precision of a double
					// 6 == adjustment in case precision is not specified,
					//   which means that the precision defaults to 6
					pszTemp = (wchar_t*)_alloca(max(nWidth, 312+nPrecision+6) * sizeof(wchar_t));

					f = va_arg(argList, double);
					swprintf( pszTemp, L"%*.*f", nWidth, nPrecision+6, f );
					nItemLen = wcslen(pszTemp);
				}
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				ASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}
	
	return nMaxLen;
}

inline int ErrorLog::_scprintf(const char* lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	int nResult = _vscprintf(lpszFormat, argList);
	va_end(argList);
	return nResult;
}

inline int ErrorLog::_scwprintf(const wchar_t* lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	int nResult = _vscwprintf(lpszFormat, argList);
	va_end(argList);
	return nResult;
}

#endif

inline void ErrorLog::Trim(LPSTR lpStr, LPCSTR lpTrim)
{
	char *pCurrent = lpStr + strlen(lpStr) - 1;
	char const *pTrim;
	for (; pCurrent > lpStr; pCurrent--)
	{
		for (pTrim = lpTrim; *pTrim; pTrim++)
		{
			if (*pCurrent == *pTrim)
			{
				*pCurrent = '\0';
				break;
			}
		}
		if (!*pTrim)
		{
			break;
		}
	}
}

inline void ErrorLog::OutputLogFile(LPCSTR lpLogInfo)
{
	if (!lpLogInfo)
	{
		return;
	}
	SYSTEMTIME SystemTime;
	TCHAR szDate[256];
	TCHAR szMutexName[MAX_PATH];
	GetLocalTime(&SystemTime);
	
	//初始化名字
#if _MSC_VER > 1200
	_stprintf_s(
		szDate, 
		_countof(szDate),
		_T("%04d%02d%02d%02d%02d"),
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute
		);
	_stprintf_s(
		szMutexName,
		_countof(szMutexName),
		_T("ERROR_LOG_MUTEX_%s"),
		szDate
		);
#else
	_stprintf(
		szDate, 
		_T("%04d%02d%02d%02d%02d"),
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute
		);
	_stprintf(
		szMutexName,
		_T("ERROR_LOG_MUTEX_%s"),
		szDate
		);
#endif 

	if (!*s_szOutputFileName)
	{
		//初始化输出目录
		TCHAR szLogDirectory[MAX_PATH];
		GetModuleFileName(NULL, szLogDirectory, MAX_PATH);
		*(_tcsrchr(szLogDirectory, '\\') + 1) = '\0';
#if _MSC_VER > 1200
		_tcscat_s(szLogDirectory, _T("Log"));
#else 
		_tcscat(szLogDirectory, _T("Log"));
#endif
		CreateDirectory(szLogDirectory, NULL);

#if _MSC_VER > 1200
		_stprintf_s(
			s_szOutputFileName, 
			_T("%s\\Log%s.txt"),
			szLogDirectory,
			szDate
			);
#else
		_stprintf(
			s_szOutputFileName, 
			_T("%s\\Log%s.txt"),
			szLogDirectory,
			szDate
			);
#endif
	}

	FILE* pFile;

	HANDLE hMutex = CreateMutex(NULL, FALSE, szMutexName);

	WaitForSingleObject(hMutex, INFINITE);

#if _MSC_VER <= 1200
	pFile = _tfopen(s_szOutputFileName, _T("ab"));
#else
	_tfopen_s(&pFile,s_szOutputFileName, _T("ab"));
#endif

	char *szBuffer;
	int iBufLen = _scprintf(\
		"[%02d:%02d:%02d.%03d]:%s",
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		SystemTime.wMilliseconds,
		lpLogInfo
		) + 32;
	szBuffer = new char[iBufLen]; 
#if _MSC_VER > 1200
	sprintf_s(
		szBuffer,
		iBufLen,
		"[%02d:%02d:%02d.%03d]:%s",
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		SystemTime.wMilliseconds,
		lpLogInfo
		);
#else
	sprintf(
		szBuffer,
		"[%02d:%02d:%02d.%03d]:%s",
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		SystemTime.wMilliseconds,
		lpLogInfo
		);
#endif
	char const szTrim[] = " \r\n\t";

	Trim(szBuffer, szTrim);

#if _MSC_VER > 1200
	strcat_s(szBuffer, iBufLen, "\r\n");
#else
	strcat(szBuffer, "\r\n");
#endif

	fwrite(szBuffer, strlen(szBuffer), 1, pFile);

	fclose(pFile);

	delete []szBuffer;

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
}

static int ErrorLog::DbgPrintA(const char* Format, ... )
{
	char *szBuffer;

	va_list args;
	va_start(args, Format);
	int iBufLen = _vscprintf(Format, args) + 1;
	szBuffer = new char[iBufLen];
#if _MSC_VER > 1200
	int n = vsprintf_s(szBuffer, iBufLen, Format, args);
#else
	int n = vsprintf(szBuffer, Format, args);
#endif
	va_end(args);
	OutputDebugStringA(szBuffer);
#if ERROR_LOG_CONSOLE
	printf(szBuffer);
#endif
#if ERROR_LOG_OUTPUT_FILE
	OutputLogFile(szBuffer);
#endif
	delete []szBuffer;
	return n;
}

static int ErrorLog::DbgPrintW(const wchar_t* Format, ...)
{
	wchar_t *szBuffer;
	va_list args;
	va_start(args, Format);
	int iBufLen = _vscwprintf(Format, args) + 1;
	szBuffer = new wchar_t[iBufLen];
#if _MSC_VER > 1200
	int n = vswprintf_s(szBuffer, iBufLen, Format, args);
#else
	int n = vswprintf(szBuffer, Format, args);
#endif
	va_end(args);
	OutputDebugStringW(szBuffer);
#if ERROR_LOG_CONSOLE
	wprintf(szBuffer);
#endif
#if ERROR_LOG_OUTPUT_FILE
	char *szAnsi;
	unsigned long ulLen, ulCount;
	ulLen = wcslen(szBuffer);
	ulCount = WideCharToMultiByte(CP_ACP, 0, szBuffer, ulLen, NULL, 0, NULL, NULL);
	if (ulCount)
	{
		szAnsi = new char[ulCount + 1];
		if (szAnsi)
		{
			WideCharToMultiByte(CP_ACP, 0, szBuffer, ulLen, szAnsi, ulCount, NULL, NULL);
			szAnsi[ulCount] = '\0';
			OutputLogFile(szAnsi);
			delete []szAnsi;
		}
	}
#endif
	delete []szBuffer;
	return n;
}

static void ErrorLog::PrintLastErrorA(const char* lpPrefix)
{
	unsigned long ulErrorCode;
	void* pMsgBuf;

	ulErrorCode = GetLastError();

	FormatMessageA(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ulErrorCode,
		0,
		(LPSTR)&pMsgBuf,
		0,
		NULL
		);

	DbgPrintA("%s 代码：%d 描述：%s\n", lpPrefix, ulErrorCode, pMsgBuf);
	LocalFree(pMsgBuf);
}

static void ErrorLog::PrintLastErrorW(const wchar_t* lpPrefix)
{
	unsigned long ulErrorCode = GetLastError();
	void* pMsgBuf = NULL;
	FormatMessageW(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ulErrorCode,
		0,
		(LPWSTR)&pMsgBuf,
		0,
		NULL
		);

	DbgPrintW(L"%s 代码：%d 描述：%s\n", lpPrefix, ulErrorCode, pMsgBuf);
	LocalFree(pMsgBuf);
}

#endif
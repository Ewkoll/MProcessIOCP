/* --------------------------------------------------------------------------------------------*/
//	文件名		:	StrConversion.h
//	目的		:	从HTTP模块中的字符串转换独立出来。
//	创建时间	:	2013-8-7 11:28
//	创建者		:	IDeath
//
//	历史记录	:
/* --------------------------------------------------------------------------------------------*/
#pragma once
#include <memory>
#include <string>
#include <Windows.h>

namespace StrConversion
{
	/*
	*	\brief	GBK转繁体字。
	*/
	__forceinline static std::string Gb2Big(std::string& strData)
	{
		std::string strBIG5Result;
		do 
		{
			try
			{
				INT nLen = MultiByteToWideChar(936, 0, strData.c_str(), -1, NULL, 0);
				std::shared_ptr<CHAR> pszGbt(new CHAR[nLen * 2 + 10]);
				ZeroMemory(pszGbt.get(), (nLen * 2 + 10) * sizeof(CHAR));
				LCMapStringA(0x0804, LCMAP_TRADITIONAL_CHINESE, strData.c_str(), -1, pszGbt.get(), nLen * 2);
				std::shared_ptr<WCHAR> pwszUnicode(new WCHAR[nLen + 10]);
				ZeroMemory(pwszUnicode.get(), sizeof(WCHAR) * (nLen + 10));
				MultiByteToWideChar(936, 0, pszGbt.get(), -1, pwszUnicode.get(), nLen);
				nLen = WideCharToMultiByte(950, 0, (PWSTR)pwszUnicode.get(), -1, NULL, 0, NULL, NULL);
				std::shared_ptr<CHAR> pszBig5(new CHAR[nLen + 10]);
				ZeroMemory(pszBig5.get(), (nLen + 10) * sizeof(CHAR));
				WideCharToMultiByte(950, 0, (PWSTR)pwszUnicode.get(), -1, pszBig5.get(), nLen, NULL, NULL);
				strBIG5Result = pszBig5.get();
			} 
			catch (...) { strBIG5Result.clear(); };
		} while (FALSE);
		return strBIG5Result;
	};

	/*
	*	\brief	繁体字转GBK编码。
	*/
	__forceinline static std::string Big2Gb(std::string& strData)
	{
		std::string strGBResult;
		do 
		{
			try
			{
				INT nLen = MultiByteToWideChar(950, 0, strData.c_str(), -1, NULL, 0);
				std::shared_ptr<WCHAR> pwszUnicode(new WCHAR[nLen + 10]);
				ZeroMemory(pwszUnicode.get(), sizeof(WCHAR) * (nLen + 10));
				MultiByteToWideChar(950, 0, strData.c_str(), -1, pwszUnicode.get(), nLen);
				std::shared_ptr<CHAR> pszGb(new CHAR[nLen + 10]);
				ZeroMemory(pszGb.get(), (nLen + 10) * sizeof(CHAR));
				WideCharToMultiByte(936, 0, (PWSTR)pwszUnicode.get(), -1, pszGb.get(), nLen, NULL, NULL);
				strGBResult = pszGb.get();
			}
			catch (...) { strGBResult.clear(); };
		} while (FALSE);
		return strGBResult;
	};

	/*
	*	\brief	单字节转双字节
	*/
	__forceinline static std::wstring AnsiToWchar(std::string& strData, UINT nCodePage = CP_ACP)
	{
		std::wstring strResult;
		try
		{
			do
			{
				INT nInputLen = (INT)strData.length();
				INT nLen = ::MultiByteToWideChar(nCodePage, 0, strData.c_str(), nInputLen, NULL, NULL);
				if(nLen <= 0)
				{
					break;
				}

				std::shared_ptr<WCHAR> pUnicodeResult(new WCHAR[nLen + 10]);
				ZeroMemory(pUnicodeResult.get(), (nLen + 10) * sizeof(WCHAR));
				::MultiByteToWideChar(nCodePage, 0, strData.c_str(), -1, pUnicodeResult.get(), nLen);
				strResult = pUnicodeResult.get();
			} while (FALSE);
		}
		catch (...){}
		return strResult;
	};
	
	/*
	*	\brief	单字节转双字节
	*/
	__forceinline static std::wstring AnsiToWchar(PSTR pszData, UINT nDataLength, UINT nCodePage = CP_ACP)
	{
		std::wstring strResult;
		try
		{
			do 
			{
				INT nInputLen = nDataLength;
				INT nLen = ::MultiByteToWideChar(nCodePage, 0, pszData, nInputLen, NULL, NULL);
				if(nLen <= 0)
				{
					break;
				}

				std::shared_ptr<WCHAR> pUnicodeResult(new WCHAR[nLen + 10]);
				ZeroMemory(pUnicodeResult.get(), (nLen + 10) * sizeof(WCHAR));
				INT nResult = ::MultiByteToWideChar(nCodePage, 0, pszData, -1, pUnicodeResult.get(), nLen);
				strResult = pUnicodeResult.get();
			} while (FALSE);
		}
		catch (...){}
		return strResult;
	};
	
	/*
	*	\brief	双字节转单字节
	*/
	__forceinline static std::string WcharToAnsi(std::wstring& strData, UINT nCodePage = CP_ACP)
	{
		std::string strResult;
		try
		{
			do
			{
				INT nInputLen = (INT)strData.length();
				INT nLen = ::WideCharToMultiByte(nCodePage, 0, strData.c_str(), -1, NULL, 0, NULL, NULL);
				if(nLen <= 0)
				{
					break;
				}

				std::shared_ptr<CHAR> pUnicodeResult(new CHAR[nLen + 10]);
				ZeroMemory(pUnicodeResult.get(), nLen + 10);
				::WideCharToMultiByte(nCodePage, 0, strData.c_str(), -1, pUnicodeResult.get(), nLen, NULL, NULL);
				strResult = pUnicodeResult.get();
			} while (FALSE);
		}
		catch (...){}
		return strResult;
	};
	
	/*
	*	\brief	双字节转单字节
	*/
	__forceinline static std::string WcharToAnsi(PWSTR pszData, UINT nDataLength, UINT nCodePage = CP_ACP)
	{
		std::string strResult;
		try
		{
			do
			{
				INT nInputLen = nDataLength;
				INT nLen = ::WideCharToMultiByte(nCodePage, 0, pszData, nInputLen, NULL, 0, NULL, NULL);
				if(nLen <= 0)
				{
					break;
				}

				std::shared_ptr<CHAR> pUnicodeResult(new CHAR[nLen + 10]);
				ZeroMemory(pUnicodeResult.get(), nLen + 10);
				::WideCharToMultiByte(nCodePage, 0, pszData, -1, pUnicodeResult.get(), nLen, NULL, NULL);
				strResult = pUnicodeResult.get();
			} while (FALSE);
		}
		catch (...){}
		return strResult;
	};

	/*
	*	\brief	字符串转UTF8格式。
	*/
	__forceinline static std::string StringToUTF8(std::string& strData)
	{
		std::string strResult;
		try
		{
			std::wstring szTemp = AnsiToWchar(strData);
			strResult = WcharToAnsi(szTemp, CP_UTF8);
		}
		catch (...){ }
		return strResult;
	};

	/*
	*	\brief	UTF8转回当前Local下的字符。
	*/
	__forceinline static std::string UTF8ToString(std::string& strData)
	{
		std::string strResult;
		try
		{
			std::wstring szTemp = AnsiToWchar(strData, CP_UTF8);
			strResult = WcharToAnsi(szTemp);
		}
		catch (...){ }
		return strResult;
	};
	
	/*
	 *	转换需要用到的全局变量相关定义。
	*/
	#define IsHexNum(cHex) ((cHex >= '0' && cHex <= '9') || (cHex >= 'A' && cHex <= 'F') || (cHex >= 'a' && cHex <= 'f'))
	static CHAR g_szUnsafe[] = ("\"<>%\\^[]`+$,@;!#");
	static CHAR g_chHexChar[] = ("0123456789ABCDEF");

	/*
	 *	转换发生异常。
	*/
	class ConversionException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{ 
			return "StrConversion Failed";
		}
	};

	/*
	*	\brief	字节转16进制字符串。
	*/
	__forceinline std::string HexToCharA(BYTE bData)
	{
		char szChar[3] = { 0 };
		try
		{
			div_t div_result = div(bData, 0x10);
			szChar[0] = g_chHexChar[div_result.quot];
			szChar[1] = g_chHexChar[div_result.rem];
		}
		catch (...) {}
		return std::string(szChar);
	}

	/*
	*	\brief	字节转16进制字符串。
	*/
	__forceinline std::wstring HexToCharW(BYTE bData)
	{
		return AnsiToWchar(HexToCharA(bData));
	}

	/*
	 *	\brief	字节转16进制字符串。
	*/
	__forceinline VOID HexToCharA(CHAR szChar[2], BYTE bData)
	{
		try
		{
			div_t div_result = div(bData, 0x10);
			szChar[0] = g_chHexChar[div_result.quot];
			szChar[1] = g_chHexChar[div_result.rem];
		}
		catch (...){}
	};

	/*
	*	\brief	字节转16进制字符串。
	*/
	__forceinline VOID HexToCharW(WCHAR szChar[2], BYTE bData)
	{
		std::wstring wHex = HexToCharW(bData);
		if (2 == wHex.length())
		{
			szChar[0] = wHex.at(0);
			szChar[1] = wHex.at(1);
		}
	};



	/*
	 *	\brief	字符串值转16进制字符串。
	*/
	__forceinline std::string StrToHexStr(std::string& strInputData)
	{
		std::string strResult;
		UINT nLength = (UINT)strInputData.length();
		for (UINT nIndex = 0; nIndex < nLength; ++nIndex)
		{
			CHAR cData = strInputData.at(nIndex);
			strResult += HexToCharA(cData & 0x00FF);
		}
		return strResult;
	}
	
	/*
	 *	\brief	16进制字符数组转数值。
	*/
	__forceinline BYTE CharToHex(CONST CHAR szChar[2], BOOL bThrow = FALSE)
	{
		BYTE nResult = 0;
		for (INT nIndex = 0; nIndex < 2; ++nIndex)
		{	
			nResult = nResult << 4;
			if (szChar[nIndex] >= '0' && szChar[nIndex] <= '9')
			{
				nResult += (szChar[nIndex] - '0');
				continue;
			}
			if (szChar[nIndex] >= 'A' && szChar[nIndex] <= 'F')
			{
				nResult += (szChar[nIndex] - 'A' + 0xA);
				continue;
			}
			if (szChar[nIndex] >= 'a' && szChar[nIndex] <= 'f')
			{
				nResult += (szChar[nIndex] - 'a' + 0xA);
				continue;
			}
			if (bThrow)
			{
				throw ConversionException();
			}
		}
		return nResult;
	};

	/*
	 *	\brief	字符串值转16进制字符串。
	*/
	__forceinline std::string HexStrToStr(std::string& strInputData)
	{
		std::string strResult;
		try
		{
			INT nLength = (INT)strInputData.length();
			std::shared_ptr<BYTE> pBuffer(new BYTE[nLength + 10]);
			ZeroMemory(pBuffer.get(), nLength + 10);
			
			PBYTE pBufferStart = pBuffer.get();
			LPCSTR pSrcStart = strInputData.c_str();
			for (INT nIndex = 0; nIndex < nLength; nIndex += 2, pSrcStart += 2)
			{
				*pBufferStart++ = CharToHex(pSrcStart);
			}
			strResult = (CHAR*)pBuffer.get();
		}
		catch (...){}
		return strResult;
	};
	
	/*
	 *	\brief	Unicode解码
	*/
	__forceinline std::string UnicodeDecode(std::string& szInputData)
	{
		std::string szResult;
		for (DWORD nIndex = 0; nIndex < szInputData.length(); ++nIndex)
		{
			CHAR cData = szInputData.at(nIndex);
			if (cData != '\\')
			{
				szResult += cData;
			}
			else
			{
				CHAR cDataFlag = szInputData.at(nIndex + 1);
				if (cDataFlag == 'u' || cDataFlag == 'U')
				{
					std::string  szTransLateData(szInputData.c_str() + 2 + nIndex, 4);
					szResult += HexStrToStr(szTransLateData);
					nIndex += 4;
				}
				else
				{
					szResult += cData;
					szResult += cDataFlag;
				}
				nIndex += 1;
			}
		}
		return szResult;
	}

	/*
	 *	\brief	Unicdoe编码
	*/
	__forceinline std::string UnicodeEncode(std::string& strInputData)
	{	
		std::wstring strWInputData = StrConversion::AnsiToWchar(strInputData);
		std::wstring szWResult;
		for (DWORD nIndex = 0; nIndex < strWInputData.length(); ++nIndex)
		{
			WCHAR cData = strWInputData.at(nIndex);
			szWResult += L"\\u";
			szWResult += HexToCharW(cData & 0x00FF);
			szWResult += HexToCharW(cData >> 8 & 0x00FF);
		}
		return StrConversion::WcharToAnsi(szWResult);
	}

	/*
	 *	\brief	URL编码中判断是否需要转码。
	*/
	__forceinline BOOL IsUnsafe(CHAR cCompare)
	{
		BOOL bFind = FALSE;
		for (INT nIndex = 0; nIndex < sizeof(g_szUnsafe) / sizeof(g_szUnsafe[0]); ++nIndex)
		{
			if (g_szUnsafe[nIndex] == cCompare)
			{
				bFind = TRUE;
				break;
			}
		}
		
		INT nAsciiValue = (INT) cCompare;
		if (bFind == FALSE &&  nAsciiValue > 32 && nAsciiValue < 123)
		{
			return FALSE;
		}
		return TRUE;
	}

	/*
	 *	\brief	URL编码
	*/
	__forceinline std::string URLEncode(std::string& strURL)
	{
		std::string strResult;
		std::string strUTF8 = StrConversion::StringToUTF8(strURL);
		INT nLength = (INT)strUTF8.length();
		do
		{
			if (nLength <= 0)
			{
				break;
			}
			
			for (INT nIndex = 0; nIndex < nLength; ++nIndex)
			{
				CHAR cData = 0; cData = strUTF8.at(nIndex);
				if (!IsUnsafe(cData))
				{
					strResult += cData;
				}
				else
				{
					strResult += "%" + HexToCharA(cData & 0x00FF);
				}
			}
		} while (FALSE);
		return strResult;
	}

	/*
	 *	\brief	URL解码
	*/
	__forceinline std::string URLDecode(std::string& strURL)
	{
		std::string strResult;
		INT nIndex = 0, nLength = (INT)strURL.length();
		do 
		{
			if (nLength <= 0)
			{
				break;
			}

			std::shared_ptr<BYTE> pBuffer(new BYTE[(nLength + 1) * 2]);
			ZeroMemory(pBuffer.get(), (nLength + 1) * 2);
			PBYTE pSrcStart = pBuffer.get();
			while (nIndex < nLength)
			{
				if (nIndex <= nLength - 3 && strURL[nIndex] == '%' && IsHexNum(strURL[nIndex + 1]) && IsHexNum(strURL[nIndex + 2]))
				{
					CHAR szBuffer[4] = { strURL[nIndex + 1], strURL[nIndex + 2], 0, 0 };
					*pSrcStart++ = CharToHex(szBuffer, TRUE);
					nIndex += 2;
				}
				else if(strURL[nIndex] == '+')
				{
					*pSrcStart++ = '\20';
				}
				else
				{
					*pSrcStart++ = strURL[nIndex] & 0x00FF;
				}
				nIndex++;
			}
			strResult = StrConversion::UTF8ToString(std::string((PSTR)pBuffer.get()));
		} while (FALSE);
		return strResult;
	}
}
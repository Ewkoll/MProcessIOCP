#include "ConvertIMPL.h"
#include "StrConversion.h"

bool CConvertIMPL::StringToUTF8String(std::string& strInput, std::string& strOutput)
{
	strOutput = StrConversion::StringToUTF8(strInput);
	return true;
}

bool CConvertIMPL::UTF8StringToString(std::string& strInput, std::string& strOutput)
{
	strOutput = StrConversion::UTF8ToString(strInput);
	return true;
}

bool CConvertIMPL::WideStringToString(std::wstring& strInput, std::string& strOutput)
{
	strOutput = StrConversion::WcharToAnsi(strInput);
	return true;
}

bool CConvertIMPL::StringToWideString(std::string& strInput, std::wstring& strOutput)
{
	strOutput = StrConversion::AnsiToWchar(strInput);
	return true;
}
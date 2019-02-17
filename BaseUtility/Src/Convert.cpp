#include "Convert.h"
#include "ConvertIMPL.h"

CConvert::CConvert()
	: m_pIMPL(new CConvertIMPL)
{

}

CConvert::~CConvert()
{
	if (NULL != m_pIMPL)
	{
		delete m_pIMPL;
		m_pIMPL = NULL;
	}
}

bool CConvert::StringToUTF8String(std::string& strInput, std::string& strOutput)
{
	return m_pIMPL->StringToUTF8String(strInput, strOutput);
}

bool CConvert::UTF8StringToString(std::string& strInput, std::string& strOutput)
{
	return m_pIMPL->UTF8StringToString(strInput, strOutput);
}

bool CConvert::WideStringToString(std::wstring& strInput, std::string& strOutput)
{
	return m_pIMPL->WideStringToString(strInput, strOutput);
}

bool CConvert::StringToWideString(std::string& strInput, std::wstring& strOutput)
{
	return m_pIMPL->StringToWideString(strInput, strOutput);
}
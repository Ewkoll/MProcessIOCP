#include "ConsoleLogIMPL.h"
#include <xlocbuf>
#include "Common/Global.h"

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		return TRUE;
	default:
		return FALSE;
	}
}

void HandleConsole(bool bHandle)
{
	if (bHandle)
	{
		HWND hwnd = ::GetConsoleWindow();
		if (hwnd != NULL)
		{
			HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
			if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
	SetConsoleCtrlHandler(HandlerRoutine, bHandle == true);
}

CConsoleLogIMPL::CConsoleLogIMPL()
	: m_bInit(false)
	, m_hOutput(INVALID_HANDLE_VALUE)
{
	if (AllocConsole())
	{
		m_bInit = true;
		m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	else
	{
		if (5 == GetLastError())
		{
			m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		}
	}

	if (NULL != m_hOutput)
	{
		HandleConsole(false);
	}
}

CConsoleLogIMPL::~CConsoleLogIMPL()
{
	if (m_bInit)
	{
		HandleConsole(false);
		FreeConsole();
	}
}

void CConsoleLogIMPL::WriteMSG(const std::string& strLevelName, const std::string& strLog)
{
	if (m_hOutput != INVALID_HANDLE_VALUE)
	{
		std::string strLogData = strLog;
		if (strLog[strLog.length() - 1] != '\n')
		{
			strLogData += "\r\n";
		}

		if (strLevelName == "INFO")
		{
			strLogData = strLevelName + "  " + strLogData;
			SetConsoleTextAttribute(m_hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		else if (strLevelName == "DEBUG")
		{
			strLogData = strLevelName + " " + strLogData;
			SetConsoleTextAttribute(m_hOutput, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}
		else if (strLevelName == "ERROR")
		{
			strLogData = strLevelName + " " + strLogData;
			SetConsoleTextAttribute(m_hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		}

		WriteConsoleA(m_hOutput, strLogData.c_str(), (DWORD)strLogData.length(), NULL, NULL);
		fflush(stdout);
	}
}
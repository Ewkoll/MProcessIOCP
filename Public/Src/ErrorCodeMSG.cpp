#include <sstream>
#include <Windows.h>
#include "ErrorCodeMSG.h"

std::string ErrorCode2Text(DWORD dwErrorCode)
{
	return ErrorCode2TextV2(dwErrorCode, std::string());
}

std::string ErrorCode2TextV2(DWORD dwErrorCode, std::string strFile /* = __FILE__*/ , int nLineCount /* = __LINE__*/)
{
	std::string strErrorMSG;
	switch (dwErrorCode)
	{
	case WSAEFAULT:
		strErrorMSG = "WSAEFAULT			The buf parameter is not completely contained in a valid part of the user address space.";
		break;
	case WSAENOTCONN:
		strErrorMSG = "WSAENOTCONN			The socket is not connected.";
		break;
	case WSAEINTR:
		strErrorMSG = "WSAEINTR				The (blocking) call was canceled through WSACancelBlockingCall.	";
		break;
	case WSAENOTSOCK:
		strErrorMSG = "WSAENOTSOCK			The descriptor s is not a socket.";
		break;
	case WSANOTINITIALISED:
		strErrorMSG = "WSANOTINITIALISED:	A successful WSAStartup call must occur before using this function.";
		break;
	case WSAENETDOWN:
		strErrorMSG = "WSAENETDOWN			The network subsystem has failed.";
		break;
	case WSAEINPROGRESS:
		strErrorMSG = "WSAEINPROGRESS		A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
		break;
	case WSAENETRESET:
		strErrorMSG = " WSAENETRESET		The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.";
		break;
	case WSAEOPNOTSUPP:
		strErrorMSG = "WSAEOPNOTSUPP		MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.";
		break;
	case WSAESHUTDOWN:
		strErrorMSG = "WSAESHUTDOWN			The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.";
		break;
	case WSAEWOULDBLOCK:
		strErrorMSG = "WSAEWOULDBLOCK		The socket is marked as nonblocking and the receive operation would block.	";
		break;
	case WSAEMSGSIZE:
		strErrorMSG = "WSAENOTSOCK			The message was too large to fit into the specified buffer and was truncated.";
		break;
	case WSAEINVAL:
		strErrorMSG = "WSAEINVAL			The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.";
		break;
	case WSAECONNABORTED:
		strErrorMSG = "WSAECONNABORTED		The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.";
		break;
	case WSAETIMEDOUT:
		strErrorMSG = "WSAETIMEDOUT			The connection has been dropped because of a network failure or because the peer system failed to respond.	";
		break;
	case WSAECONNRESET:
		strErrorMSG	= "WSAECONNRESET		The virtual circuit was reset by the remote side executing a hard or abortive close."; 
		break;
	}
	
	if (strErrorMSG.empty())
	{
		LPVOID lpvMessageBuffer = NULL;
		DWORD dwResult = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
									    FORMAT_MESSAGE_FROM_SYSTEM |
									    FORMAT_MESSAGE_IGNORE_INSERTS,
									    NULL,
									    dwErrorCode,
									    MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
									    (LPSTR)&lpvMessageBuffer,
									    0,
									    NULL);
		if (dwResult != 0 && lpvMessageBuffer != NULL)
		{
			std::stringstream StringStream;
			StringStream << (CHAR*)lpvMessageBuffer;
			strErrorMSG = StringStream.str();
			LocalFree(lpvMessageBuffer);
		}
	}

	std::stringstream StringStream;
	if (strFile.empty())
	{
		StringStream << " 错误信息:" << strErrorMSG;
	}
	else
	{
		StringStream << "文件:" << strFile << " 行号:" << nLineCount << " 错误信息:" << strErrorMSG;
	}
	strErrorMSG = StringStream.str();
	return strErrorMSG;
}
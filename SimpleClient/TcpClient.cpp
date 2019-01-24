#include "TcpClient.h"

#include <iostream>
#include <winsock2.h>

using namespace std;

/*static*/ bool CTcpClient::m_bWinsockIsUp = false;

/*static*/ bool CTcpClient::WinsockInit()
{
	bool bResult = false;

	if (m_bWinsockIsUp)
	{
		bResult = true; //already up
	}
	else
	{
		WSADATA WSAData;
		int iErr = WSAStartup(MAKEWORD(2,0), &WSAData);

		if (iErr == 0)
		{
			bResult = true;
		}
		else
		{
			cout << "Error on Winsock init (Error code = ) " << iErr << endl;
		}
	}

	return bResult;
}
/*static*/ bool CTcpClient::WinsockCleanup()
{
	bool bResult = false;

	if (!m_bWinsockIsUp)
	{
		bResult = true; //already down
	}
	else
	{
		if (WSACleanup() == 0)
		{
			bResult = true;
		}
		else
		{
			int iErr = WSAGetLastError();
			cout << "Error on Winsock init (Error code = ) " << iErr << endl;
		}
	}

	return bResult;
}

CTcpClient::CTcpClient(const string& stServerIp, const int iServerPort) :
	m_stServerIp(stServerIp),
	m_iServerPort(iServerPort),
	m_bIsConnected(false),
	m_socket(NULL)
{

}

/*virtaul*/ CTcpClient::~CTcpClient()
{

}

bool CTcpClient::Connect()
{
	bool bResult = false;

	if (m_bIsConnected)
	{
		//already connected
		bResult = true;
	}
	else
	{
		if (WinsockInit())
		{
			SOCKADDR_IN addr;

			m_socket = socket(AF_INET, SOCK_STREAM, 0);

			addr.sin_addr.s_addr = inet_addr(m_stServerIp.c_str());
			addr.sin_family = AF_INET;
			addr.sin_port = htons(m_iServerPort);

			if (connect(m_socket, (SOCKADDR*)&addr, sizeof(addr)) != 0)
			{
				const int iErrCode = WSAGetLastError();
				cout << "Error connecting to server (Error code = ) " << iErrCode << endl;
			}
			else
			{
				cout << "Connected to server!" << endl;

				bResult = true;
				m_bIsConnected = true;
			}
		}
	}

	return bResult; 
}

bool CTcpClient::Disconnect()
{
	bool bResult = false;

	if (!m_bIsConnected)
	{
		//already disconnected
		bResult = true;
	}
	else
	{
		if (closesocket(m_socket) == 0)
		{
			bResult = true;
		}
		else
		{
			int iErr = WSAGetLastError();
			cout << "Error closing socket (Error code = ) " << iErr << endl;
		}
	}

	return bResult; 
}

bool CTcpClient::SendData(const string& stData)
{
	bool bResult = false;

	if (m_bIsConnected)
	{
		if (send(m_socket, stData.c_str(), stData.size(), 0) == stData.size())
		{
			bResult = true;
		}
		else
		{
			int iErr = WSAGetLastError();
			cout << "Error sending data through socket (Error code = ) " << iErr << endl;
		}
	}

	return bResult;
}

bool CTcpClient::SendData(const wstring& wstData)
{
	bool bResult = false;

	if (m_bIsConnected)
	{
		size_t buffer_size;
		wcstombs_s(&buffer_size, NULL, 0, wstData.c_str(), _TRUNCATE);

		if (buffer_size > 0)
		{
			char* buffer = (char*)malloc(buffer_size);
			wcstombs_s(&buffer_size, buffer, buffer_size, wstData.c_str(), _TRUNCATE);
			string stToSend = string(buffer);

			if (send(m_socket, stToSend.c_str(), stToSend.size(), 0) != stToSend.size())
			{
				int iErr = WSAGetLastError();
				cout << "Error sending data through socket (Error code = ) " << iErr << endl;
			}
			else
			{
				bResult = true;
			}

			free(buffer);
		}
	}

	return bResult;
}

#pragma once

#include <string>

typedef unsigned int SOCKET;

class CTcpClient
{

private:

	std::string					m_stServerIp;
	int							m_iServerPort;
	bool						m_bIsConnected;
	SOCKET						m_socket;

public:

	CTcpClient(const std::string& stServerIp, const int iServerPort);
	virtual ~CTcpClient();

	bool						Connect();
	bool						Disconnect();
	bool						SendData(const std::string& stData);
	bool						SendData(const std::wstring& wstData);

	static bool					WinsockInit();
	static bool					WinsockCleanup();

	static bool					m_bWinsockIsUp;
};

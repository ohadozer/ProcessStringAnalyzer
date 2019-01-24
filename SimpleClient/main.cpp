#include <stdio.h>
#include <string>
#include <iostream>

#include "TcpClient.h"
#include "ProcessMemoryAnalyzer.h"
#include "BlockingQueue.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Usage: SimpleClient <ServerIP> <ServerPort>" << endl; 
	}
	else
	{
		string stServerIp = string(argv[1]);
		int iServerPort = strtol(argv[2], NULL, 10);

		if (iServerPort <= 0)
		{
			cout << "Illegal parameter(s) value" << endl;
		}
		else
		{
			CTcpClient client(stServerIp, iServerPort);

			if (client.Connect())
			{
				CProcessMemoryAnalyzer* pProcessMemoryAnalyzer = NULL;

				try
				{
					pProcessMemoryAnalyzer = new CProcessMemoryAnalyzer(&client, wstring(L"explorer.exe"));
				}
				catch(const exception&)
				{
					cout << "error initializing memory analyzer for explorer.exe" << endl;
					return 1;
				}

				try
				{
					pProcessMemoryAnalyzer->Analyze();
				}
				catch(const exception&)
				{
					cout << "error occured while analyzing explorer.exe, analysis has been interrupted" << endl;
				}

				delete pProcessMemoryAnalyzer;

				client.Disconnect();
			}

			CTcpClient::WinsockCleanup();
		}
	}

	return 0;
}

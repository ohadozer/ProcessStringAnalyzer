#pragma once

#include <string>
#include <Windows.h>

class CTcpClient;

class CProcessMemoryAnalyzer
{

private:

	HANDLE			m_hProcToAnalyze;
	CTcpClient*		m_pServerEndpoint;

	bool			InitProcessMemoryAnalyzer(int iPid);
	DWORD			FindProcessIdByProcessName(std::wstring& wstProcName);
	DWORD			GetSystemPageSize();
	void			AnalyzeAscii(const byte* pBuf, const DWORD dwBufSize);
	void			AnalyzeUnicode(const byte* pBuf, const DWORD dwBufSize);
	bool			IsAsciiPrintableChar(const byte chByte);
	bool			IsUnicodePrintableChar(const WCHAR wchChar);

public:

	CProcessMemoryAnalyzer(CTcpClient* pServerEndpoint, const int iPid);
	CProcessMemoryAnalyzer(CTcpClient* pServerEndpoint, std::wstring& wstPname);
	virtual ~CProcessMemoryAnalyzer();

	void			Analyze();
};

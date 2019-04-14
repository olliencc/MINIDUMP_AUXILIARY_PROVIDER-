// 
// MiniDump
// April 2019
// Ollie Whitehouse
//

#include <Windows.h>
#include <strsafe.h>
#include <minidumpapiset.h>
#include <processthreadsapi.h>

#pragma comment(lib, "Dbghelp.lib")

//
// Entry point
//
int main()
{


	// Write the dump
	// https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/crash-dump-analysis
	// https://docs.microsoft.com/en-us/windows/desktop/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump

	BOOL bMiniDumpSuccessful;
	WCHAR szPath[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	const WCHAR* szAppName = L"MiniDump";
	const WCHAR* szVersion = L"v1.0";
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;

	GetLocalTime(&stLocalTime);
	GetTempPath(dwBufferSize, szPath);

	StringCchPrintf(szFileName, MAX_PATH, L"%s%s", szPath, szAppName);
	CreateDirectory(szFileName, NULL);

	StringCchPrintf(szFileName, MAX_PATH, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
		szPath, szAppName, szVersion,
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
		GetCurrentProcessId(), GetCurrentThreadId());
	
	
	fwprintf(stdout, L"[!] Wrote %s\n", szFileName);

	hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
		hDumpFile, MiniDumpWithDataSegs, NULL, NULL, NULL);
}

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	WCHAR szPath[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	const WCHAR* szAppName = L"MiniDump";
	const WCHAR* szVersion = L"v1.0";
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	HANDLE hFoo;

    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:	


			GetLocalTime(&stLocalTime);
			GetTempPath(dwBufferSize, szPath);

			StringCchPrintf(szFileName, MAX_PATH, L"%s%s", szPath, szAppName);
			CreateDirectory(szFileName, NULL);

			StringCchPrintf(szFileName, MAX_PATH, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.run",
				szPath, szAppName, szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());

			hFoo = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, NULL, NULL);
			if (hFoo != NULL){
				WriteFile(hFoo, L"Run\n", wcslen(L"Run\n"),NULL,NULL);
				CloseHandle(hFoo);
			}
			break;
		case DLL_THREAD_ATTACH:
			GetLocalTime(&stLocalTime);
			GetTempPath(dwBufferSize, szPath);

			StringCchPrintf(szFileName, MAX_PATH, L"%s%s", szPath, szAppName);
			CreateDirectory(szFileName, NULL);

			StringCchPrintf(szFileName, MAX_PATH, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.run",
				szPath, szAppName, szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());

			hFoo = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, NULL, NULL, NULL);
			if (hFoo != NULL) {
				WriteFile(hFoo, L"Run\n", wcslen(L"Run\n"), NULL, NULL);
				CloseHandle(hFoo);
			}
			break;
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


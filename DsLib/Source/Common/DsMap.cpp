#include "DsPch.h"
#ifndef _DS_MAP_
#include "Common/DsMap.h"
#endif // !_DS_MAP_

#include <windows.h>

using namespace DsLib;

//static 
std::wstring DsUtil::GetCurrentPath()
{
	std::wstring ret = L"";
	const DWORD nBufferLength = 1024;
	WCHAR lpBuffer[nBufferLength];
	if (::GetCurrentDirectory(nBufferLength, lpBuffer) != 0) {
		ret = lpBuffer;
	}
	
	return ret;
}

//static 
std::wstring DsUtil::GetExePath()
{
	std::wstring ret = L"";
	const DWORD nBufferLength = 1024;
	WCHAR lpBuffer[nBufferLength];
	if (::GetModuleFileName(NULL, lpBuffer, nBufferLength) != 0) {
		ret = lpBuffer;
	}

	return ret;
}

#include "DsPch.h"

#ifndef _DS_PATH_
#include "System/DsPath.h"
#endif

using namespace DsLib;
using namespace std;

static std::wstring s_assetPath;
static bool s_isSetAssetPath = false;

//static
bool DsPath::IsSetAssetPath()
{
	return s_isSetAssetPath;
}

//static
void DsPath::SetAssetPath(const wstring path)
{
	s_assetPath = path;
	s_isSetAssetPath = true;
}

//static
wstring DsPath::GetAssetPath()
{
	return s_assetPath;
}

//static
wstring DsPath::GetCurrentPath()
{
	wstring ret = L"";
	const DWORD nBufferLength = 1024;
	WCHAR buf[nBufferLength];
	if (0 != ::GetCurrentDirectoryW(nBufferLength, buf)) {
		ret = wstring(buf);
	}
	return ret;
}

//static
wstring DsPath::GetFileName(wstring path)
{
	wstring ret = path;

	auto end = path.end();
	auto fileBegin= path.begin();
	for (auto it = path.begin(); end != it; ++it) {
		if (((*it) == L'/') || ((*it) == L'\\')) {
			fileBegin = it + 1;
		}
	}

	ret.assign(fileBegin, end);
	return ret;
}

//static
wstring DsPath::ConvAssetPath(wstring path)
{
	wstring ret = L"";
	if (DsPath::IsSetAssetPath()) {
		wstring assetPath = DsPath::GetAssetPath();
		wstring fileName = DsPath::GetFileName(path);
		ret = assetPath + L"\\" + fileName;
	}
	else {
		ret = path;
	}
	return ret;
}

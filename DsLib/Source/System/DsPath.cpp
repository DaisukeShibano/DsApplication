#include "DsPch.h"

#ifndef _DS_PATH_
#include "System/DsPath.h"
#endif

#include <codecvt> 

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
wstring DsPath::GetDirectoryName(wstring path)
{
	wstring ret = path;

	auto end = path.rend();
	for (auto it = path.rbegin(); it != end; ++it) {
		if (((*it) == L'/') || ((*it) == L'\\')) {
			ret.assign(path.begin(), it.base());
			break;
		}
	}

	return ret;
}

//static
wstring DsPath::GetFileName(wstring path)
{
	wstring ret = path;

	auto end = path.rend();
	for (auto it = path.rbegin(); it != end; ++it) {
		if (((*it) == L'/') || ((*it) == L'\\')) {
			ret.assign(it.base(), path.end());
			break;
		}
	}

	return ret;
}

//static
wstring DsPath::GetFileNameWithoutExt(wstring path)
{
	wstring ret = path;

	auto nameIt = path.begin();
	{
		auto end = path.rend();
		for (auto it = path.rbegin(); it != end; ++it) {
			if (((*it) == L'/') || ((*it) == L'\\')) {
				nameIt = it.base();
				break;
			}
		}
	}

	auto extIt = path.end();
	{
		auto end = path.rend();
		for (auto it = path.rbegin(); it != end; ++it) {
			if (((*it) == '.') && (it != path.rbegin())) {
				extIt = it.base()-1;
				break;
			}
		}
	}

	ret.assign(nameIt, extIt);
	return ret;
}

//static
wstring DsPath::GetFileExt(wstring path)
{
	wstring ret = L"";

	auto end = path.rend();
	for (auto it = path.rbegin(); it != end; ++it) {
		if ( ((*it) == '.') && (it != path.rbegin()) ) {
			//base()ÇÕàÍÇ¬å„ÇÎÇï‘Ç∑
			ret.assign(it.base(), path.end());
			break;
		}
	}
	return ret;
}

//static
wstring DsPath::GetFileExtWithDot(wstring path)
{
	wstring ret = L"";

	auto end = path.rend();
	for (auto it = path.rbegin(); it != end; ++it) {
		if (((*it) == '.') && (it != path.rbegin())) {
			//base()ÇÕàÍÇ¬å„ÇÎÇï‘Ç∑
			ret.assign(it.base()-1, path.end());
			break;
		}
	}
	return ret;
}

//aa\\bb\\cc_***.dd  ÇÃ *** Çí«â¡Ç∑ÇÈ
//static
wstring DsPath::AddSuffix(std::wstring path, std::wstring suffix)
{
	return DsPath::GetDirectoryName(path) + DsPath::GetFileNameWithoutExt(path) + suffix + DsPath::GetFileExtWithDot(path);
}

//static
wstring DsPath::ToWstring(string src)
{
	wstring ret = L"";

	//îÒêÑèßÇÁÇµÇ¢
	//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	//ret = cv.from_bytes(src);

	size_t retNum = 0;
	const size_t wcharNum = src.size()+1;
	wchar_t* pRetBuff = new wchar_t[wcharNum];
	const char* srcChar = src.c_str();
	const size_t srcCharNum = src.length();

	setlocale(LC_CTYPE, "jpn");
	errno_t error = mbstowcs_s(&retNum, pRetBuff, wcharNum, srcChar, srcCharNum);
	if (0 == error) {
		ret = pRetBuff;
	}

	delete pRetBuff;
	return ret;
}

//static
string DsPath::ToString(std::wstring src)
{
	string ret = "";
	
	size_t retNum = 0;
	const size_t charNum = src.size() + 1;
	char* pRetBuff = new char[charNum];
	const wchar_t* srcWchar = src.c_str();
	const size_t srcWcharNum = src.length();

	setlocale(LC_CTYPE, "jpn");
	errno_t error = wcstombs_s(&retNum, pRetBuff, charNum, srcWchar, srcWcharNum);
	if (0 == error) {
		ret = pRetBuff;
	}

	delete pRetBuff;
	return ret;
}
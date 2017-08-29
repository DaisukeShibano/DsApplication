#include "DsPch.h"
#ifndef _DS_FILE_
#include "System/DsFile.h"
#endif

#include < locale.h >

using namespace std;
using namespace DsLib;

DsFile::DsFile(const char *_Filename,
	ios_base::openmode _Mode,
	int _Prot)
	: m_wifstream()
{
	string srcPath = _Filename;

	setlocale(LC_CTYPE, "jpn");
	size_t retNum = 0;
	const size_t wcharNum = 1024;
	wchar_t retBuff[wcharNum];
	const char* srcChar = srcPath.c_str();
	const size_t srcCharNum = srcPath.length();
	errno_t error = mbstowcs_s(&retNum, retBuff, wcharNum, srcChar, srcCharNum);
	if (0 == error) {
		wstring path = DsPath::ConvAssetPath(retBuff);
		m_wifstream.open(path, _Mode, _Prot);
	}
}

DsFile::DsFile(const wchar_t *_Filename,
	ios_base::openmode _Mode,
	int _Prot)
	: m_wifstream()
{
	wstring path = DsPath::ConvAssetPath(_Filename);
	m_wifstream.open(path, _Mode, _Prot);
}

DsFile::~DsFile()
{
	m_wifstream.close();
}
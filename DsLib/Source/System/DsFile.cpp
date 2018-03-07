#include "DsPch.h"
#ifndef _DS_FILE_
#include "System/DsFile.h"
#endif
#include <sys/stat.h>
#include < locale.h >

using namespace std;
using namespace DsLib;

DsFile::DsFile(const char *_Filename, ios_base::openmode _Mode, int _Prot)
#if defined __USE_IFSTREAM0
	: m_ifstream()
#elif defined __USE_IFSTREAM1
	: m_ifstream()
	, m_bufTop()
	, m_pReadBuf(NULL)
#elif defined __USE_IFSTREAM2
	: m_pBufTop(NULL)
	, m_pReadBuf(NULL)
	, m_bufSize(0)
#endif
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
#if defined __USE_IFSTREAM0
		m_ifstream.open(path, _Mode, _Prot);
#elif defined __USE_IFSTREAM1
		m_ifstream.open(path, _Mode, _Prot);
#elif defined __USE_IFSTREAM2
		FILE* fp=NULL;
		if (0 == _wfopen_s(&fp, path.c_str(), L"rb")) {
			struct stat st;
			int fd = _fileno(fp);
			fstat(fd, &st);
			m_pBufTop = new char[st.st_size];
			fread_s(m_pBufTop, st.st_size, sizeof(char), st.st_size, fp);
			m_pReadBuf = m_pBufTop;
			m_bufSize = st.st_size;
			fclose(fp);
		}
#endif
		_Init();
	}
}

DsFile::DsFile(const wchar_t *_Filename, ios_base::openmode _Mode, int _Prot)
#if defined __USE_IFSTREAM0
	: m_ifstream()
#elif defined __USE_IFSTREAM1
	: m_ifstream()
	, m_bufTop()
	, m_pReadBuf(NULL)
#elif defined __USE_IFSTREAM2
	: m_pBufTop(NULL)
	, m_pReadBuf(NULL)
	, m_bufSize(0)
#endif

{
	wstring path = DsPath::ConvAssetPath(_Filename);
#if defined __USE_IFSTREAM0
	m_ifstream.open(path, _Mode, _Prot);
#elif defined __USE_IFSTREAM1
	m_ifstream.open(path, _Mode, _Prot);
#elif defined __USE_IFSTREAM2
	FILE* fp = NULL;
	if (0 == _wfopen_s(&fp, path.c_str(), L"rb")) {
		struct stat st;
		int fd = _fileno(fp);
		fstat(fd, &st);
		m_pBufTop = new char[st.st_size];
		fread_s(m_pBufTop, st.st_size, sizeof(char), st.st_size, fp);
		m_pReadBuf = m_pBufTop;
		m_bufSize = st.st_size;
		fclose(fp);
	}
#endif
	_Init();
}

DsFile::~DsFile()
{
#if defined __USE_IFSTREAM0
	m_ifstream.close();
#elif defined __USE_IFSTREAM1

#elif defined __USE_IFSTREAM2
	delete m_pBufTop; m_pBufTop = NULL;
#endif
}

void DsFile::Read(char* pBuf, std::streamsize size)
{
#if defined __USE_IFSTREAM0
	m_ifstream.read(pBuf, size);
#elif defined __USE_IFSTREAM1
	memcpy_s(pBuf, size, m_pReadBuf, size);
	m_pReadBuf += size;
#elif defined __USE_IFSTREAM2
	memcpy_s(pBuf, size, m_pReadBuf, size);
	m_pReadBuf += size;
#endif
}

bool DsFile::IsFail() const
{
#if defined __USE_IFSTREAM0
	return m_ifstream.fail();
#elif defined __USE_IFSTREAM1
	return (m_bufTop.empty());
#elif defined __USE_IFSTREAM2
	return (NULL == m_pBufTop);
#endif
}


void DsFile::Seekg(long long int _Off, SEEK_TYPE _Way)
{
#if defined __USE_IFSTREAM0
	if (_Way == SEEK_TYPE::BEG) {
		m_ifstream.seekg(_Off, std::ios::beg);
	}
	else if(_Way == SEEK_TYPE::CUR){
		m_ifstream.seekg(_Off, std::ios::cur);
	}
#elif defined __USE_IFSTREAM1
	if (_Way == SEEK_TYPE::BEG) {
		m_pReadBuf = m_bufTop.data() + _Off;
	}
	else if (_Way == SEEK_TYPE::CUR) {
		m_pReadBuf = m_pReadBuf + _Off;
	}
#elif defined __USE_IFSTREAM2
	if (_Way == SEEK_TYPE::BEG) {
		m_pReadBuf = m_pBufTop + _Off;
	}
	else if (_Way == SEEK_TYPE::CUR) {
		m_pReadBuf = m_pReadBuf + _Off;
	}
#endif
}

bool DsFile::GetLine(std::string& str)
{
	bool ret = false;

	const char* end = m_pBufTop + m_bufSize;
	const char* read = m_pReadBuf;
	while (end != read)
	{
		if( ((*read) == '\n') ||
			((*read) == EOF) ){
			ret = true;
			break;
		}
		else {
			++read;
		}
	}

	if (ret) {
		str.assign(m_pReadBuf, static_cast<ds_uint64>(read- m_pReadBuf) );
		m_pReadBuf = (char*)read;
		++m_pReadBuf;
	}
	
	return ret;
}


void DsFile::_Init()
{
#if defined __USE_IFSTREAM0

#elif defined __USE_IFSTREAM1
	std::istreambuf_iterator<char> it(m_ifstream);
	std::istreambuf_iterator<char> last;
	m_bufTop.assign(it, last);
	m_pReadBuf = m_bufTop.data();
	m_ifstream.close();
#elif defined __USE_IFSTREAM2

#endif

}
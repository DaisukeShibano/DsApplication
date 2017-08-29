#ifndef _DS_FILE_
#define _DS_FILE_

namespace DsLib
{
	class DsFile
	{
	public:
		DsFile(const char *_Filename,
			std::ios_base::openmode _Mode = std::ios_base::in,
			int _Prot = (int)std::ios_base::_Openprot);
		DsFile(const wchar_t *_Filename,
			std::ios_base::openmode _Mode = std::ios_base::in,
			int _Prot = (int)std::ios_base::_Openprot);

		virtual ~DsFile();


	private:
		std::wifstream m_wifstream;
	};


}

#endif
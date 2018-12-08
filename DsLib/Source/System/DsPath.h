#ifndef _DS_PATH_
#define _DS_PATH_

namespace DsLib
{
	class DsPath
	{
	public:
		static
		bool IsSetAssetPath();

		static
		void SetAssetPath(const std::wstring path);

		static
		std::wstring GetAssetPath();

		static
		std::wstring ConvAssetPath(std::wstring path);

		static
		std::wstring GetCurrentPath();

		static
		std::wstring GetDirectoryName(std::wstring path);

		static
		std::wstring GetFileName(std::wstring path);

		static
		std::wstring GetFileNameWithoutExt(std::wstring path);

		static
		std::wstring GetFileExt(std::wstring path);

		static
		std::wstring GetFileExtWithDot(std::wstring path);

		//aa\\bb\\cc_***.dd  ‚Ì *** ‚ð’Ç‰Á‚·‚é
		static
		std::wstring AddSuffix(std::wstring path, std::wstring suffix);

		static
		std::wstring ToWstring(std::string src);

		static
		std::string ToString(std::wstring src);

	};
}

#endif
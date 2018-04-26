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
		std::wstring GetCurrentPath();

		static
		std::wstring GetFileName(std::wstring path);

		static
		std::wstring ConvAssetPath(std::wstring path);

	};
}

#endif
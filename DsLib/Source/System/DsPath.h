#ifndef _DS_PATH_
#define _DS_PATH_

namespace DsLib
{
	class DsPath
	{
	public:
		static
		void SetAssetPath(const std::wstring path);

		static
		std::wstring GetAssetPath();
	};
}

#endif
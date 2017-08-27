#include "DsPch.h"

#ifndef _DS_PATH_
#include "System/DsPath.h"
#endif

using namespace DsLib;

static std::wstring s_assetPath;

//static
void DsPath::SetAssetPath(const std::wstring path)
{
	s_assetPath = path;
}

//static
std::wstring DsPath::GetAssetPath()
{
	return s_assetPath;
}
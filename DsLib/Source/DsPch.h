// 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#ifndef _DS_PCH_H_
#define _DS_PCH_H_

#include "targetver.h"
#include <assert.h>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <fstream>
#include <regex>
#include <locale>

#ifdef _WIN32
#include <Windows.h>
#endif
#ifndef _DS_DEFINE_
#include "DsDefine.h"
#endif
#ifndef _DS_SYS_DEFINE_H_
#include "System/DsSysDefine.h"
#endif
#ifndef _DS_ERROR_H_
#include "Error/DsError.h"
#endif
#ifndef _DS_UTIL_
#include "DsUtil.h"
#endif // !_DS_UTIL_
#ifndef _DS_MATH_H_
#include "Math/DsMath.h"
#endif
#ifndef _DS_DBG_SYS_H_
#include "System/DsDbgSys.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif
#ifndef _DS_PERF_H_
#include "System/DsPerf.h"
#endif
#ifndef _DS_PATH_
#include "System/DsPath.h"
#endif
#ifndef _DS_FILE_
#include "System/DsFile.h"
#endif

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。

#define ENUM_TO_STR(var) #var


#endif

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。

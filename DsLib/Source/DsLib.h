#ifndef _DS_LIB_H_
#define _DS_LIB_H_

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

#ifdef _WIN32
#include <Windows.h>
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
#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#ifndef _DS_MAIN_LOOP_H_
#include "System/DsMainLoop.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif
#ifndef _DS_RENDER_H_
#include "Graphics/Render/DsRender.h"
#endif
#ifndef _DS_RENDER_IMAGE_H_
#include "Graphics/Render/DsRenderCamCaptureImage.h"
#endif
#ifndef _DS_GRAPHIC_TOOL_H_
#include "Graphics/Render/Tool/DsRenderTool.h"
#endif
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif
#ifndef _DS_ANIM_SKELETON_MODIFIER_
#include "Animation/DsAnimSkeletonModifier.h"
#endif
#ifndef _DS_ANIM_CONTROLLER_H_
#include "Animation/ActionRequest/DsAnimController.h"
#endif
#ifndef _DS_ACT_REQ_H_
#include "Animation/ActionRequest/DsActReq.h"
#endif
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif
#ifndef _DS_MOUSE_H_
#include "Mouse/DsMouse.h"
#endif
#ifndef _DS_PERF_H_
#include "System/DsPerf.h"
#endif


#endif
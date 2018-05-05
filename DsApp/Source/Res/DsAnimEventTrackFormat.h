#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#define _DS_ANIM_EVENR_TRACK_FORMAR_

/*
#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#include "Res/DsAnimEventTrackFormat.h"
#endif
*/

namespace DsApp
{
	enum DS_ANIM_ET_ACTION_TYPE
	{
		TRACE_EFFECT,
		DAMAGE,
		ACTION_TYPE_NUM,
	};

	struct DS_ANIM_ET_TRACE_EFFECT
	{
		int dmyPolyId[2];
		int effectId;
	};

	struct DS_ANIM_ET_DAMAGE
	{
		int dmyPolyId[2];
		int damageId;
	};

	struct DS_ANIM_ET_HEADER
	{
		int version;
	};

	//1バー
	struct DS_ANIM_ET_PARAM
	{
		int paramType;
		float startTime;
		float endTime;
		union {
			long long paramOffset;
			DS_ANIM_ET_TRACE_EFFECT* pEffect;
			DS_ANIM_ET_DAMAGE* pDamage;
		};
	};

	//1アニメ
	struct DS_ANIM_ET_PARAM_SET
	{
		int paramNum;
		union {
			long long animNameOffset;
			char* pAnimName;
		};
		union {
			long long paramsOffset;
			DS_ANIM_ET_PARAM* pParams;
		};
	};

	//全体
	struct DS_ANIM_ET_DATA
	{
		DS_ANIM_ET_HEADER header;
		int paramSetNum;
		union {
			long long modelPathOffset;
			char* pModelPath;
		};
		union {
			long long paramSetOffset;
			DS_ANIM_ET_PARAM_SET* pParamSet;
		};
	};

}

#endif
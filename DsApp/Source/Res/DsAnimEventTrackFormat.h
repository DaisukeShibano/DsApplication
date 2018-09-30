#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#define _DS_ANIM_EVENR_TRACK_FORMAR_

/*
#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#include "Res/DsAnimEventTrackFormat.h"
#endif
*/

namespace DsApp
{
	enum class DS_ANIM_ET_ACTION_TYPE : int
	{
		TRACE_EFFECT,
		SOUND_EFFECT,
		DAMAGE,
		CANCEL_ACTION_TIMING,
		ANIM_INTERPOLATION,
		ACTION_INPUT,
		NO_HIT_DAMAGE,
		DISABLE_TURN,
		DISABLE_GRAVITY,
		SUPER_ARMOR,
		ACTION_TYPE_NUM,
	};
	struct DS_ANIM_ET_TRACE_EFFECT
	{
		int dmyPolyId[2];
		int effectId;
	};
	struct DS_ANIM_ET_SOUND_EFFECT
	{
		int dmyPolyId;
		int soundId;
	};
	struct DS_ANIM_ET_DAMAGE
	{
		int dmyPolyId[2];
		int damageId;
	};
	struct DS_ANIM_ET_CANCEL_ACTION_TIMING
	{
		int action;
	};
	struct DS_ANIM_ET_ANIM_INTERPOLATION
	{
		float time;
	};
	struct DS_ANIM_ET_ACTION_INPUT
	{
		int actionType;
	};
	struct DS_ANIM_ET_NO_HIT_DAMAGE
	{
	};
	struct DS_ANIM_ET_DISABLE_TURN
	{
	};
	struct DS_ANIM_ET_DISABLE_GRAVITY
	{
	};
	struct DS_ANIM_ET_SUPER_ARMOR
	{
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
			DS_ANIM_ET_TRACE_EFFECT* pSound;
			DS_ANIM_ET_DAMAGE* pDamage;
			DS_ANIM_ET_CANCEL_ACTION_TIMING* pCancel;
			DS_ANIM_ET_ANIM_INTERPOLATION* pAnimInterpolation;
			DS_ANIM_ET_ACTION_INPUT* pActionInput;
			DS_ANIM_ET_NO_HIT_DAMAGE* pNoHitDamage;
			DS_ANIM_ET_DISABLE_TURN* pDisableTurn;
			DS_ANIM_ET_DISABLE_GRAVITY* pDisableGravity;
			DS_ANIM_ET_SUPER_ARMOR* pSuperArmor;
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
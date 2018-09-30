#pragma once
#include "Setting.h"


enum ACTION_TYPE
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

struct ACTION_TYPE_NAME
{
	ACTION_TYPE type;
	const wchar_t* name;
};

static const ACTION_TYPE_NAME ACTION_NAME_ARRAY[]=
{
	{ TRACE_EFFECT,				L"軌跡エフェクト" },
	{ SOUND_EFFECT,				L"SE" },
	{ DAMAGE,					L"ダメージ" },
	{ CANCEL_ACTION_TIMING,		L"キャンセル可能タイミング" },
	{ ANIM_INTERPOLATION,		L"アニメ補間" },
	{ ACTION_INPUT,				L"入力受付" },
	{ NO_HIT_DAMAGE,			L"ダメージすり抜け" },
	{ DISABLE_TURN,				L"旋回無効" },
	{ DISABLE_GRAVITY,			L"重力無効"},
	{ SUPER_ARMOR ,				L"スーパーアーマー"},
};

ref struct INT_PARAM : public System::Object
{
	System::Int32^ value;
	System::String^ name;
};

ref struct DECIMAL_PARAM : public System::Object
{
	System::Decimal^ value;
	System::String^ name;
};

public ref struct PARAM_BASE abstract : public System::Object
{
	virtual char* GetParamStaticBuff()=0;
};



struct ET_TRACE_EFFECT
{
	int dmyPolyId[2];
	int effectId;
};
ref struct TRACE_EFFECT_PARAM : PARAM_BASE
{
	TRACE_EFFECT_PARAM()
	{
		dmyPolyId0.name = gcnew System::String(L"dmyPolyId0");
		dmyPolyId0.value = gcnew System::Int32(-1);
		dmyPolyId1.name = gcnew System::String(L"dmyPolyId1");
		dmyPolyId1.value = gcnew System::Int32(-1);
		effectId.name = gcnew System::String(L"effectId");
		effectId.value = gcnew System::Int32(-1);
	}
	INT_PARAM dmyPolyId0;
	INT_PARAM dmyPolyId1;
	INT_PARAM effectId;
	const int paramNum = 3;

	virtual char* GetParamStaticBuff() override
	{
		static ET_TRACE_EFFECT ret;
		ret.dmyPolyId[0] = (*dmyPolyId0.value);
		ret.dmyPolyId[1] = (*dmyPolyId1.value);
		ret.effectId = (*effectId.value);
		return (char*)(&ret);
	}
	void Set(const ET_TRACE_EFFECT& param) {
		dmyPolyId0.value = param.dmyPolyId[0];
		dmyPolyId1.value = param.dmyPolyId[1];
		effectId.value = param.effectId;
	}
};

struct ET_SOUND_EFFECT
{
	int dmyPolyId;
	int soundId;
};
ref struct SOUND_EFFECT_PARAM : PARAM_BASE
{
	SOUND_EFFECT_PARAM()
	{
		dmyPolyId.name = gcnew System::String(L"dmyPolyId");
		dmyPolyId.value = gcnew System::Int32(-1);
		soundId.name = gcnew System::String(L"soundId");
		soundId.value = gcnew System::Int32(-1);
	}
	INT_PARAM dmyPolyId;
	INT_PARAM soundId;
	const int paramNum = 2;

	virtual char* GetParamStaticBuff() override
	{
		static ET_SOUND_EFFECT ret;
		ret.dmyPolyId = (*dmyPolyId.value);
		ret.soundId = (*soundId.value);
		return (char*)(&ret);
	}
	void Set(const ET_SOUND_EFFECT& param) {
		dmyPolyId.value = param.dmyPolyId;
		soundId.value = param.soundId;
	}
};


struct ET_DAMAGE
{
	int dmyPolyId[2];
	int damageId;
};
ref struct DAMAGE_PARAM : PARAM_BASE
{
	DAMAGE_PARAM()
	{
		dmyPolyId0.name = gcnew System::String(L"dmyPolyId0");
		dmyPolyId0.value = gcnew System::Int32(-1);
		dmyPolyId1.name = gcnew System::String(L"dmyPolyId1");
		dmyPolyId1.value = gcnew System::Int32(-1);
		damageId.name = gcnew System::String(L"damageId");
		damageId.value = gcnew System::Int32(-1);
	}
	INT_PARAM dmyPolyId0;
	INT_PARAM dmyPolyId1;
	INT_PARAM damageId;
	const int paramNum = 3;

	virtual char* GetParamStaticBuff() override
	{
		static ET_TRACE_EFFECT ret;
		ret.dmyPolyId[0] = (*dmyPolyId0.value);
		ret.dmyPolyId[1] = (*dmyPolyId1.value);
		ret.effectId = (*damageId.value);
		return (char*)(&ret);
	}
	void Set(const ET_DAMAGE& param) {
		dmyPolyId0.value = param.dmyPolyId[0];
		dmyPolyId1.value = param.dmyPolyId[1];
		damageId.value = param.damageId;
	}
};

struct ET_CANCEL_ACTION_TIMING
{
	int actionType;
};
ref struct CANCEL_ACTION_TIMING_PARAM : PARAM_BASE
{
	CANCEL_ACTION_TIMING_PARAM()
	{
		actionType.name = gcnew System::String(L"actionType");
		actionType.value = gcnew System::Int32(-1);
	}
	INT_PARAM actionType;
	const int paramNum = 1;

	virtual char* GetParamStaticBuff() override
	{
		static ET_CANCEL_ACTION_TIMING ret;
		return (char*)(&ret);
	}
	void Set(const ET_CANCEL_ACTION_TIMING& param) {
		actionType.value = param.actionType;
	}
};

struct ET_ANIM_INTERPOLATION
{
	int time;
};
ref struct ANIM_INTERPOLATION_PARAM : PARAM_BASE
{
	ANIM_INTERPOLATION_PARAM()
	{
		time.name = gcnew System::String(L"time");
		time.value = gcnew System::Decimal(0.0);
	}
	DECIMAL_PARAM time;
	const int paramNum = 1;

	virtual char* GetParamStaticBuff() override
	{
		static ET_ANIM_INTERPOLATION ret;
		return (char*)(&ret);
	}
	void Set(const ET_ANIM_INTERPOLATION& param) {
		time.value = System::Decimal(param.time);
	}
};


struct ET_ACTION_INPUT
{
	int actionType;
};
ref struct ACTION_INPUT_PARAM : PARAM_BASE
{
	ACTION_INPUT_PARAM()
	{
		actionType.name = gcnew System::String(L"actionType");
		actionType.value = gcnew System::Int32(-1);
	}
	INT_PARAM actionType;
	const int paramNum = 1;

	virtual char* GetParamStaticBuff() override
	{
		static ET_ACTION_INPUT ret;
		return (char*)(&ret);
	}
	void Set(const ET_ACTION_INPUT& param) {
		actionType.value =param.actionType;
	}
};

struct ET_NO_HIT_DAMAGE
{

};
ref struct NO_HIT_DAMAGE_PARAM : PARAM_BASE
{

	const int paramNum = 0;

	virtual char* GetParamStaticBuff() override
	{
		static ET_NO_HIT_DAMAGE ret;
		return (char*)(&ret);
	}
	void Set(const ET_NO_HIT_DAMAGE& param) {
	}
};

struct ET_DISABLE_TURN
{

};
ref struct DISABLE_TURN_PARAM : PARAM_BASE
{
	const int paramNum = 0;

	virtual char* GetParamStaticBuff() override
	{
		static ET_DISABLE_TURN ret;
		return (char*)(&ret);
	}
	void Set(const ET_DISABLE_TURN& param) {
	}
};

struct ET_DISABLE_GRAVITY
{

};
ref struct DISABLE_GRAVITY_PARAM : PARAM_BASE
{
	const int paramNum = 0;

	virtual char* GetParamStaticBuff() override
	{
		static ET_DISABLE_GRAVITY ret;
		return (char*)(&ret);
	}
	void Set(const ET_DISABLE_GRAVITY& param) {
	}
};


struct ET_SUPER_ARMOR
{

};
ref struct SUPER_ARMOR_PARAM : PARAM_BASE
{
	const int paramNum = 0;

	virtual char* GetParamStaticBuff() override
	{
		static ET_SUPER_ARMOR ret;
		return (char*)(&ret);
	}
	void Set(const ET_SUPER_ARMOR& param) {
	}
};




struct ET_HEADER
{
	int version;
};

//1バー
struct ET_PARAM
{
	int paramType;
	float startTime;
	float endTime;
	union {
		long long paramOffset;
		ET_TRACE_EFFECT* pEffect;
		ET_SOUND_EFFECT* pSound;
		ET_DAMAGE* pDamage;
		ET_CANCEL_ACTION_TIMING* pCancelTiming;
		ET_ANIM_INTERPOLATION* pAnimInterpolation;
		ET_ACTION_INPUT* pActionInput;
		ET_NO_HIT_DAMAGE* pNoHitDamage;
		ET_DISABLE_TURN* pDisableTurn;
		ET_DISABLE_GRAVITY* pDisableGravity;
		ET_SUPER_ARMOR* pSuperArmor;
	};
};

//1アニメ
struct ET_PARAM_SET
{
	int paramNum;
	union {
		long long animNameOffset;
		char* pAnimName;
	};
	union {
		long long paramsOffset;
		ET_PARAM* pParams;
	};
};

//全体
struct ET_DATA
{
	ET_HEADER header;
	int paramSetNum;
	union {
		long long modelPathOffset;
		char* pModelPath;
	};
	union {
		long long paramSetOffset;
		ET_PARAM_SET* pParamSet;
	};
};






//元データ1アニメ
ref struct EDIT_TRACK_SET : public System::Object
{
	System::String^ animName;
	System::Collections::Generic::List<System::Windows::Forms::TextBox^>^ tracks = gcnew System::Collections::Generic::List<System::Windows::Forms::TextBox^>();
};

//元データ全体
ref struct EDIT_DATA : public System::Object
{
	System::String^ modelPath;
	System::Collections::Generic::List<EDIT_TRACK_SET^>^ data = gcnew System::Collections::Generic::List<EDIT_TRACK_SET^>();
};
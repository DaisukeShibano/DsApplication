#pragma once
#include "Setting.h"


enum ACTION_TYPE
{
	TRACE_EFFECT,
	DAMAGE,
	ACTION_TYPE_NUM,
};

struct ACTION_TYPE_NAME
{
	ACTION_TYPE type;
	const wchar_t* name;
};

static const ACTION_TYPE_NAME ACTION_NAME_ARRAY[]=
{
	{ TRACE_EFFECT,		L"軌跡エフェクト" },
	{ DAMAGE,			L"ダメージ" },
};

ref struct INT_PARAM : public System::Object
{
	System::Int32^ value;
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
		ET_DAMAGE* pDamage;
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
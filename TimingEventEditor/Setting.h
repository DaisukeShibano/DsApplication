#pragma once

static const int FPS = 60;
static const float FPSf = static_cast<float>(FPS);
static const float SPF = 1.0f/FPSf;
static const float TICK_SEC = SPF;//1目盛りあたりの秒数
static const int TICK_WDTH = 5;//1目盛りあたりの幅
static const float MAX_TIME = 5.0f;//最大秒数
static const int MAX_TICK_NUM = static_cast<int>(MAX_TIME / TICK_SEC);
static const int HEIGHT_BAR = 24;//バーの高さ


enum ACTION_TYPE
{
	TRACE_EFFECT,
	DAMAGE,
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

ref struct TRACE_EFFECT_PARAM : public System::Object
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
};

ref struct DAMAGE_PARAM : public System::Object
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
};
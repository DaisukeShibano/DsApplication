#pragma once
#include "Setting.h"


enum ACTION_TYPE
{
	TRACE_EFFECT,
	SOUND_EFFECT,
	DAMAGE,
	CANCEL_ACTION_TIMING,
	ACTION_TYPE_NUM,
};

struct ACTION_TYPE_NAME
{
	ACTION_TYPE type;
	const wchar_t* name;
};

static const ACTION_TYPE_NAME ACTION_NAME_ARRAY[]=
{
	{ TRACE_EFFECT,				L"�O�ՃG�t�F�N�g" },
	{ SOUND_EFFECT,				L"SE" },
	{ DAMAGE,					L"�_���[�W" },
	{ CANCEL_ACTION_TIMING,		L"�L�����Z���\�^�C�~���O" },
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
};
ref struct CANCEL_ACTION_TIMING_PARAM : PARAM_BASE
{
	CANCEL_ACTION_TIMING_PARAM()
	{
	}
	const int paramNum = 0;

	virtual char* GetParamStaticBuff() override
	{
		static ET_CANCEL_ACTION_TIMING ret;
		return (char*)(&ret);
	}
	void Set(const ET_CANCEL_ACTION_TIMING& param) {
	}
};


struct ET_HEADER
{
	int version;
};

//1�o�[
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
	};
};

//1�A�j��
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

//�S��
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






//���f�[�^1�A�j��
ref struct EDIT_TRACK_SET : public System::Object
{
	System::String^ animName;
	System::Collections::Generic::List<System::Windows::Forms::TextBox^>^ tracks = gcnew System::Collections::Generic::List<System::Windows::Forms::TextBox^>();
};

//���f�[�^�S��
ref struct EDIT_DATA : public System::Object
{
	System::String^ modelPath;
	System::Collections::Generic::List<EDIT_TRACK_SET^>^ data = gcnew System::Collections::Generic::List<EDIT_TRACK_SET^>();
};
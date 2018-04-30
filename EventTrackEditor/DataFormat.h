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
	{ TRACE_EFFECT,		L"�O�ՃG�t�F�N�g" },
	{ DAMAGE,			L"�_���[�W" },
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


struct ET_HEADER
{
	int version;
};

struct ET_TRACE_EFFECT
{
	int dmyPolyId[2];
	int effectId;
};

struct ET_DAMAGE
{
	int dmyPolyId[2];
	int damageId;
};

//1�o�[
struct ET_PARAM
{
	long long paramOffset;
	int paramType;
	float startTime;
	float endTime;
};

//1�A�j��
struct ET_PARAM_SET
{
	long long animNameOffset;
	long long paramsOffset;
};

//�S��
struct ET_DATA
{
	ET_HEADER header;
	long long paramSetOffset;
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
	System::String^ animPath;
	System::Collections::Generic::List<EDIT_TRACK_SET^>^ data = gcnew System::Collections::Generic::List<EDIT_TRACK_SET^>();
};
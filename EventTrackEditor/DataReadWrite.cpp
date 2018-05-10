#include "stdafx.h"
#include "DataReadWriter.h"
#include "TagData.h"
#include "Form1.h"
#include "Simu/DsSimu.h"
#include "Simu/DsSimuUtlil.h"

using namespace System;
using namespace System::Windows;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Text;


namespace
{
	void _WirteString(System::IO::FileStream ^ fs, String^ str) {
		String^ addEnd = str + "\0";
		array<Byte>^ tmpName = Encoding::ASCII->GetBytes(addEnd);
		const int size = tmpName->Length * sizeof(char);
		fs->Write(tmpName, 0, size);
	}

	int _GetStringSize(String^ str) {
		String^ addEnd = str + "\0";
		array<Byte>^ tmpName = Encoding::ASCII->GetBytes(addEnd);
		const int size = tmpName->Length * sizeof(char);
		return size;
	}

	void _WriteInt(System::IO::FileStream ^ fs, int val) {
		fs->Write(BitConverter::GetBytes(val), 0, sizeof(val));
	}

	void _WriteByte(System::IO::FileStream ^ fs, char* pVal, int size) {
		array<unsigned char>^ data = gcnew array<unsigned char>(size);
		for (int i = 0; i < size; ++i) {
			data[i] = pVal[i];
		}
		fs->Write( data, 0, size);
	}
}

void WriteData(System::IO::FileStream ^ fs, const EDIT_DATA^ data)
{
	/*
	{
		ET_DATA
	}
	//★paramSetOffset
	{
		ET_PARAM_SET
		ET_PARAM_SET
		:
	}
	//★paramTopOffset
	{
		ET_PARAM
		ET_PARAM
		:
	}
	//★modelPathOffset
	{
		modelPath
	}
	//★paramOffset
	{
		{
			animName
			ET_HOGEHOGE
			ET_HOGEHOGE
			:
		}
		{
			animName
			ET_HOGEHOGE
			ET_HOGEHOGE
			:
		}
		:
	}
	*/
	//EDIT_DATAから保存フォーマット形式のデータを作る
	int modelPathOffset = 0;
	int paramSetOffset = 0;
	int paramTopOffset = 0;
	ET_DATA wData;
	modelPathOffset += sizeof(ET_DATA);
	paramSetOffset = modelPathOffset;
	ET_PARAM_SET* wParamSet = new ET_PARAM_SET[data->data->Count];
	modelPathOffset += sizeof(ET_PARAM_SET)*data->data->Count;
	paramTopOffset = modelPathOffset;
	int paramCount = 0;
	for each(const EDIT_TRACK_SET^ trackSet in data->data) {
		paramCount += trackSet->tracks->Count;
	}
	ET_PARAM* wParam = new ET_PARAM[paramCount];
	modelPathOffset += sizeof(ET_PARAM)*paramCount;


	wData.header.version = 0;
	wData.paramSetNum = data->data->Count;
	wData.modelPathOffset = modelPathOffset;
	wData.paramSetOffset = paramSetOffset;
	
	paramCount = 0;
	int paramOffset = modelPathOffset + _GetStringSize(data->modelPath);
	for (int idx = 0; idx < data->data->Count; ++idx) {
		const int paramNum = data->data[idx]->tracks->Count;
		wParamSet[idx].paramNum = paramNum;
		wParamSet[idx].animNameOffset = paramOffset;
		wParamSet[idx].paramsOffset= paramTopOffset;
		paramTopOffset += paramNum * sizeof(ET_PARAM);

		paramOffset += _GetStringSize(data->data[idx]->animName);
		for (int tIdx = 0; tIdx < paramNum; ++tIdx) {
			ActionBarTag^ tag = dynamic_cast<ActionBarTag^>(data->data[idx]->tracks[tIdx]->Tag);
			wParam[paramCount].paramType = tag->actionType;
			wParam[paramCount].startTime = tag->startTime;
			wParam[paramCount].endTime = tag->endTime;
			wParam[paramCount].paramOffset = paramOffset;
			paramOffset += tag->paramSize;
			++paramCount;
		}
	}
	
	//書き込む
	_WriteByte(fs, (char*)(&wData), sizeof(wData));
	_WriteByte(fs, (char*)(wParamSet), sizeof(ET_PARAM_SET)*data->data->Count);
	_WriteByte(fs, (char*)(wParam), sizeof(ET_PARAM)*paramCount);
	_WirteString(fs, data->modelPath);
	for each(EDIT_TRACK_SET^ paramSet in data->data) {
		_WirteString(fs, paramSet->animName);
		for each(TextBox^ textBox in paramSet->tracks) {
			ActionBarTag^ tag = dynamic_cast<ActionBarTag^>(textBox->Tag);
			PARAM_BASE^ paramBase = dynamic_cast<PARAM_BASE^>(tag->param);
			char* pBuf = paramBase->GetParamStaticBuff();
			const int bufSize = tag->paramSize;
			_WriteByte(fs, pBuf, bufSize);
		}
	}
}


void ReadData(System::IO::FileStream ^ fs, EventTrackEditor::Form1^ form)
{
	const int size = static_cast<int>(fs->Length);
	array<unsigned char>^ buf = gcnew array<unsigned char>(size);
	fs->Read(buf, 0, size);
	pin_ptr<unsigned char> p = &buf[0];
	const unsigned char* pBufTop = p;
	
	ET_DATA* rData = (ET_DATA*)pBufTop;
	rData->modelPathOffset += (long long)(pBufTop);
	rData->paramSetOffset += (long long)(pBufTop);

	ET_PARAM_SET* rParamSet = rData->pParamSet;
	for (int i = 0; i < rData->paramSetNum; ++i) {
		rParamSet[i].paramsOffset += (long long)(pBufTop);
		rParamSet[i].animNameOffset += (long long)(pBufTop);

		ET_PARAM* rParams = rParamSet[i].pParams;
		const char* animName = rParamSet[i].pAnimName;
		for (int pIdx = 0; pIdx < rParamSet[i].paramNum; ++pIdx) {
			rParams[pIdx].paramOffset += (long long)(pBufTop);
		}
	}

	form->SetCurrentAnim(nullptr);
	form->CreateModel(gcnew String(rData->pModelPath));
	for (int s = 0; s < rData->paramSetNum; ++s) {
		
		form->ClearActionBarNum();

		for (int p = 0; p < rData->pParamSet[s].paramNum; ++p) {
			const float startTime = rData->pParamSet[s].pParams[p].startTime;
			const float endTime = rData->pParamSet[s].pParams[p].endTime;
			const ACTION_TYPE type = static_cast<ACTION_TYPE>(rData->pParamSet[s].pParams[p].paramType);
			String^ barTypeName = GetActionBarTypeName(type);
			String^ animName = gcnew String(rData->pParamSet[s].pAnimName);
			PARAM_BASE^ barBase = form->CreareBar(startTime, endTime, barTypeName, animName);
			switch (type)
			{
			case TRACE_EFFECT:
			{
				ET_TRACE_EFFECT* pTmp = rData->pParamSet[s].pParams[p].pEffect;
				TRACE_EFFECT_PARAM^ bar = dynamic_cast<TRACE_EFFECT_PARAM^>(barBase);
				bar->Set(*pTmp);
			}
			break;
			case SOUND_EFFECT:
			{
				ET_SOUND_EFFECT* pTmp = rData->pParamSet[s].pParams[p].pSound;
				SOUND_EFFECT_PARAM^ bar = dynamic_cast<SOUND_EFFECT_PARAM^>(barBase);
				bar->Set(*pTmp);
			}
			break;
			case DAMAGE:
			{
				ET_DAMAGE* pTmp = rData->pParamSet[s].pParams[p].pDamage;
				DAMAGE_PARAM^ bar = dynamic_cast<DAMAGE_PARAM^>(barBase);
				bar->Set(*pTmp);
			}
			break;
			case CANCEL_ACTION_TIMING:
			{
				ET_CANCEL_ACTION_TIMING* pTmp = rData->pParamSet[s].pParams[p].pCancelTiming;
				CANCEL_ACTION_TIMING_PARAM^ bar = dynamic_cast<CANCEL_ACTION_TIMING_PARAM^>(barBase);
				bar->Set(*pTmp);
			}
			break;
			default:
				break;
			}
		}
	}
	form->ClearBarOnlyUI();
	form->ResetBarTime();
	

}
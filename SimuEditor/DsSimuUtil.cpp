#ifndef _DS_SIMU_UTIL_
#include "DsSimuUtlil.h"
#endif
#ifndef _DS_SIMU_H_
#include "DsSimu.h"
#endif
#ifndef _DS_DATA_FORMAT_H_
#include "DsDataFormat.h"
#endif
#include < iostream >
#include < vcclr.h >

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Text;

using namespace SimuEditor;
using namespace DsApp;
using namespace DsLib;
using namespace DsPhysics;

char* CreateCharArray(String^ str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);
	size_t convertedChars = 0;
	size_t sizeInBytes = ((str->Length + 1) * 2);
	errno_t err = 0;
	char *ch = (char *)malloc(sizeInBytes);
	err = wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);
	return ch;
}

//åªç›ÇÃFieldObjÇListVeiwItemÇ…îΩâfÇ≥ÇπÇÈ
void UpdateListViewItemParam(System::Windows::Forms::ListViewItem^ item)
{
	ObjInsFormat^data = static_cast<ObjInsFormat^>(item->Tag);
	DsFieldObj* pObj = static_cast<DsFieldObj*>(data->pFieldObj);
	const DsVec3d pos = pObj->GetPosition();
	data->posX = pos.x;
	data->posY = pos.y;
	data->posZ = pos.z;

	item->Tag = data;
}
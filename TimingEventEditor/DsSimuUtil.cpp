#include "stdafx.h"

#ifndef _DS_SIMU_UTIL_
#include "DsSimuUtlil.h"
#endif
#ifndef _DS_SIMU_H_
#include "Simu/DsSimu.h"
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

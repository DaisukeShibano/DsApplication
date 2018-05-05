#pragma once
#include "DataFormat.h"

private ref struct ActionBarTag : public System::Object
{
	System::Drawing::Point preMousePos = System::Drawing::Point(0, 0);
	ACTION_TYPE actionType;
	PARAM_BASE^ param;
	float startTime;
	float endTime;
	int paramSize;
};
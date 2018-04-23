#pragma once
#include "Setting.h"

private ref struct ActionBarTag : public System::Object
{
	System::Drawing::Point preMousePos = System::Drawing::Point(0, 0);
	ACTION_TYPE actionType;
	System::Object^ param;
};
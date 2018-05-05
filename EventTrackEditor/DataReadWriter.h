#pragma once
#include "DataFormat.h"

namespace EventTrackEditor
{
	ref class Form1;
}

void WriteData(System::IO::FileStream ^ fs, const EDIT_DATA^ data);
void ReadData(System::IO::FileStream ^ fs, EventTrackEditor::Form1^ form);

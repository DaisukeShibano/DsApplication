#pragma once
#include "MainForm.h" // [2]�ŕt����Form��.h

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	// gcnew [1]�ŕt�����v���W�F�N�g��::[2]�ŕt����Form��()
	Application::Run(gcnew SimuEditor::MainForm());
	return 0;
}
#pragma once
#include "MainForm.h" // [2]で付けたForm名.h

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	// gcnew [1]で付けたプロジェクト名::[2]で付けたForm名()
	Application::Run(gcnew SimuEditor::MainForm());
	return 0;
}
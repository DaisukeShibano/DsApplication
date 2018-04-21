// TimingEventEditor.cpp: Hauptprojektdatei.

// #include "stdafx.h"
// using namespace System;

//int main(array<System::String ^> ^args)
//{
//    Console::WriteLine(L"Hello World");
//    return 0;
//}
#include "stdafx.h"
#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
// int main(array<String^>^ args) { // Kann Fehler nach 'using namespace std;' verursachen
int main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew TimingEventEditor::Form1()); // "TimingEventEditor" noch anpassen
	return 0;
}
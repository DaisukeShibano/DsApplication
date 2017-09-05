#ifndef _DS_WINDOW_EZ_H_
#define _DS_WINDOW_EZ_H_

#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif


#if 0 //îpé~

namespace DsLib
{
	class DsCamera;
	class DsMainLoop;
	class DsSys;
	class DsMouse;
	class DsKeyboard;
}

namespace DsLib
{
	class DsWindowEz : public DsWindow
	{
	public:
		DsWindowEz();
		virtual ~DsWindowEz();

	public:
		virtual void Initialize( InitArgs& args ) override;
		virtual void Setup() override;
		virtual int GetCurrentWindowW() const override;
		virtual int GetCurrentWindowH() const override;
		virtual DsMat33d GetPerspective() const override;

	private:
		static void _Display();
		static void _Timer( int value );
		static void _Reshape( int w, int h );
		static void _Drag( int x, int y);
		static void _Click(int button, int state, int x, int y);
		static void _Wheel(int wheel, int direction, int x, int y);
		static void _Keyboard(unsigned char key, int x, int y);
		static void _KeyboardUp(unsigned char key, int x, int y);
		static void _KeyboardSp(int key, int x, int y);
	};
}

#endif

#endif
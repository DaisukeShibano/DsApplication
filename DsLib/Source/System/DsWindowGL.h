#ifndef _DS_WINDOW_GL_H_
#define _DS_WINDOW_GL_H_

#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif


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
	class DsWindowGL : public DsWindow
	{
	public:
		DsWindowGL();
		virtual ~DsWindowGL();

	public:
		virtual void Initialize(InitArgs& args) override;
		virtual void Finalize() override;
		virtual void Setup() override;
		virtual void SetHandle(int handle) override;
		virtual int GetCurrentWindowW() const override;
		virtual int GetCurrentWindowH() const override;
		virtual DsMat33d GetPerspective() const override;

	public:
		void Display();
		void Timer(int value);
		void Reshape();
		void Drag(int x, int y);
		void Click(int button, int state, int x, int y);
		void Wheel(int wheel, int x, int y);
		void Keyboard(unsigned char key, int x, int y);
		void KeyboardUp(unsigned char key, int x, int y);
		void KeyboardSp(int key, int x, int y);

	private:
		void _SetHandle(int handle);

	private:
		int m_hWnd;
		int m_hDC;
		int m_hGLRC;
	};
}

#endif
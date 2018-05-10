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
		virtual void Display() override;
		virtual void Timer(int value) override;
		virtual void Reshape() override;
		virtual void Drag(int x, int y) override;
		virtual void Click(int button, int state, int x, int y) override;
		virtual void Wheel(int wheel, int x, int y) override;
		virtual void KeyboardDown(ds_uint8 key, int x, int y) override;
		virtual void KeyboardUp(ds_uint8 key, int x, int y) override;
		virtual void KeyboardSpDown(int key, int x, int y) override;
		virtual void KeyboardSpUp(int key, int x, int y) override;

	private:
		void _SetHandle(ds_uint64 handle);

	private:
		ds_uint64 m_hWnd;
		ds_uint64 m_hDC;
		ds_uint64 m_hGLRC;
	};
}

#endif
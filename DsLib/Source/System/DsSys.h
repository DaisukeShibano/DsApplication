#ifndef _DS_SYS_H_
#define _DS_SYS_H_

namespace DsLib
{
	class DsWindow;
	class DsRender;
	class DsCamera;
	class DsMainLoop;
	class DsMouse;
	class DsKeyboard;
	class DsResource;
}

namespace DsLib
{
	struct DsSysArgs
	{
		DsSysArgs()
			: argc(0)
			, argv(0)
			, pLoop(0)
			, pConfPath(0)
			, pResourcePath(0)
			, windowType(0)
			, windowHandle(0)
		{}
		int argc;
		char** argv;
		DsMainLoop* pLoop;
		char* pConfPath;
		char* pResourcePath;
		int windowType;
		int windowHandle;
	};

	class DsSys
	{
	public:
		DsSys();
		virtual ~DsSys();

	public:
		void Setup(DsSysArgs& args);
		void Update();

	public:
		bool IsSetUp() const;
		double GetDt() const;

	public:
		DsRender& RefRender() { return *m_pRender; }
		const DsRender& RefRender() const { return *m_pRender; }
		DsCamera& RefCam() { return *m_pCam; }
		const DsCamera& RefCam() const { return *m_pCam; }
		DsMouse& RefMouse();
		const DsMouse& RefMouse() const;
		DsKeyboard& RefKeyboard();
		const DsKeyboard& RefKeyboard() const;
		DsWindow& RefWindow(){ return *m_pWindow; }
		const DsWindow& RefWindow() const { return *m_pWindow; }

	private:
		bool m_isSetUp;
		DsWindow* m_pWindow;
		DsRender* m_pRender;
		DsCamera* m_pCam;

	};
}

#endif
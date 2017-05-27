#ifndef _DS_WINDOW_H_
#define _DS_WINDOW_H_

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
	class DsWindow
	{
	public:
		struct InitArgs
		{
			InitArgs()
				:argc(0)
				, argv(0)
				, pCam(0)
				, pLoop(0)
				, pSys(0)
				, handle(0)
			{}
			int argc;
			char **argv;
			DsCamera* pCam;
			DsMainLoop* pLoop;
			DsSys* pSys;
			int handle;
		};

	protected:
		enum
		{
			INIT_WINDOW_W = 640,
			INIT_WINDOW_H = 480,
			DEFAULT_FPS = 60,
		};

	public:
		DsWindow();
		virtual ~DsWindow();

	public:
		void Update(double dt);

	public:
		virtual void Initialize(InitArgs& args);
		virtual void Finalize(){};
		virtual void Setup();
		virtual void SetHandle(int handle){}
		virtual int GetCurrentWindowW() const { return m_windowW; }
		virtual int GetCurrentWindowH() const { return m_windowH; }
		virtual DsMat33d GetPerspective() const { return DsMat33d::Identity(); }

	public:
		int GetWindowW() const { return m_windowW; }
		int GetWindowH() const { return m_windowH; }
		DsVec3d GetHalfNormalizeScreenCoord(const DsVec2i& pos) const;
		DsVec3d GetNormalizeScreenCoord(const DsVec2i& pos) const;

		double GetDt() const { return static_cast<double>(m_mspf); }
		DsMouse& RefMouse() { return *m_pMouse; }
		DsKeyboard& RefKeyboard(){ return *m_pKeyboard; }

	protected:
		int m_windowW;
		int m_windowH;
		float m_mspf;
		DsCamera* m_pCam;
		DsMainLoop* m_pLoop;
		DsSys* m_pSys;
		DsMouse* m_pMouse;
		DsKeyboard* m_pKeyboard;
	};
}

#endif
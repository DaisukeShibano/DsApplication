#ifndef _DS_EZ_WINDOW_H_
#define _DS_EZ_WINDOW_H_

namespace DsLib
{
	class DsCamera;
	class DsMainLoop;
	class DsSys;
}

namespace DsLib
{
	class DsEzWindow
	{
	public:
	struct InitArgs
	{
		InitArgs()
		:argc(0)
		,argv(NULL)
		,pCam(NULL)
		,pLoop(NULL)
		,pSys(NULL)
		{}
		int argc;
		char **argv;
		DsCamera* pCam;
		DsMainLoop* pLoop;
		DsSys* pSys;
	};

	private:
		enum
		{
			INIT_WINDOW_W = 640,
			INIT_WINDOW_H = 480,
			DEFAULT_FPS = 60,
		};

	private:
		DsEzWindow();

	public:
		virtual ~DsEzWindow();

	public:
		static DsEzWindow& GetIns()
		{
			static DsEzWindow ret;
			return ret;
		}

	public:
		void Initialize( InitArgs& args );
		void SetUpCallBack();

	public:
		int GetWindowW() const { return m_windowW; }
		int GetWindowH() const { return m_windowH; }
		double GetDt() const { return static_cast<double>(m_mspf); }

	private:
		static void _Display();
		static void _Timer( int value );
		static void _Reshape( int w, int h );
		static void _Drag( int x, int y);

	private:
		int m_windowW;
		int m_windowH;
		float m_mspf;
		DsCamera* m_pCam;
		DsMainLoop* m_pLoop;
		DsSys* m_pSys;
	};
}

#endif
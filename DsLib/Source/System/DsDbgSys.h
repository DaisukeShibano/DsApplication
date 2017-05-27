#ifndef _DS_DBG_SYS_H_
#define _DS_DBG_SYS_H_

namespace DsLib
{
	class DsDrawCommand;
	class DsSys;
	class DsMouse;
	class DsCamera;
	class DsWindow;
}


namespace DsLib
{
	class DsDbgSys
	{
	public:
		DsDbgSys( DsSys& sys );
		virtual ~DsDbgSys();

	public:
		static void Initialize( DsSys& sys );
		static void Finalize();
		static DsDbgSys& GetIns();

	public:
		DsDrawCommand& RefDrawCom();
		DsMouse& RefMouse();
		DsCamera& RefCam();
		DsWindow& RefWindow();

	private:
		DsSys& m_sys;
	};
}

#endif
#ifndef _DS_WINDOW_MANAGER_H_
#define _DS_WINDOW_MANAGER_H_

namespace DsLib
{
	class DsWindow;
}

namespace DsLib
{
	enum DS_WINDOW_SYSTEM_TYPE
	{
		DS_WINDOW_SYSTEM_TYPE_GLUT=0,
		DS_WINDOW_SYSTEM_TYPE_GL,
	};

	class DsWindowManager
	{
	public:
		DsWindowManager();
		virtual ~DsWindowManager();

	public:
		/*
		@par ウィンドウ作成。
		@return 作成したウィンドウハンドル(HWND)
		*/
		static ds_uint64 MainWindowCreate(ds_uint64 _hInstance, char* lpCmdLine, int nCmdShow);
		/*
		@par メッセージループ。
		@return 終了したか
		*/
		static bool MainWindowUpdate(ds_uint64 _hwnd, DsSys& sys);
		
		static DsWindow* Create( DS_WINDOW_SYSTEM_TYPE type );
	};	
}

#endif
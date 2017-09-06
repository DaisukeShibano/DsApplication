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
		@par ウィンドウ作成。中身はWIN32のテンプレで面倒くさい時用。別に必ず叩く必要は無い
		return 作成したウィンドウハンドル(HWND)
		*/
		static ds_uint64 MainWindowCreate(ds_uint64 _hInstance, char* lpCmdLine, int nCmdShow);
		/*
		@par メッセージループ。中身はWIN32のテンプレで面倒くさい時用。
　			 DsWindowManager::MainWindowCreateを使った時はこの関数を使う必要がある
		return 終了したか
		*/
		static bool MainWindowUpdate(ds_uint64 _hwnd, DsSys& sys);
		
		static DsWindow* Create( DS_WINDOW_SYSTEM_TYPE type );
	};	
}

#endif
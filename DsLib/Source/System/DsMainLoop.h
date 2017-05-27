#ifndef _DS_MAIN_LOOP_H_
#define _DS_MAIN_LOOP_H_

namespace DsLib
{
	class DsDrawCommand;
	class DsRenderTool;
	class DsSys;
}

namespace DsLib
{
	struct DsMainLoopArgs
	{
		DsMainLoopArgs(DsDrawCommand& com, DsRenderTool& rtool, DsSys& _sys, double _dt )
		: drawCom(com)
		, rendTool(rtool)
		, sys(_sys)
		, dt(_dt)
		{}
		DsDrawCommand& drawCom;
		DsRenderTool& rendTool;
		DsSys& sys;
		const double dt;
	};

	class DsMainLoop
	{
	public:
		//DsSysの準備が整った後に呼ばれる
		virtual void Initialize(DsMainLoopArgs& args){};

	//1FPSごとに呼ばれる関数
	public:
		//DsWindowEzクラス更新前に呼ばれる関数
		virtual void BeforeWindowUpdate( DsMainLoopArgs& args ){};
		//DsWindowEzクラス更新後に呼ばれる関数
		virtual void AfterWindowUpdate(){};
	};
}

#endif
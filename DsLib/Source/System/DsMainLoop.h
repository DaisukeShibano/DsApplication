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
		//DsSys�̏�������������ɌĂ΂��
		virtual void Initialize(DsMainLoopArgs& args){};

	//1FPS���ƂɌĂ΂��֐�
	public:
		//DsWindowEz�N���X�X�V�O�ɌĂ΂��֐�
		virtual void BeforeWindowUpdate( DsMainLoopArgs& args ){};
		//DsWindowEz�N���X�X�V��ɌĂ΂��֐�
		virtual void AfterWindowUpdate(){};
	};
}

#endif
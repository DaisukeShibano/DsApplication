#ifndef _DS_GRAPHIC_TOOL_H_
#define _DS_GRAPHIC_TOOL_H_

namespace DsLib
{
	class DsDrawCommand;
	class DsCamera;
	class DsToolCloud;
}

namespace DsLib
{
	class DsRenderTool
	{
	public:
		DsRenderTool( DsDrawCommand& com, DsCamera& cam );
		virtual ~DsRenderTool();

	public:
		DsToolCloud& RefCloud() { return *m_pCloud; }

	private:
		DsToolCloud* m_pCloud;

	};
}

#endif
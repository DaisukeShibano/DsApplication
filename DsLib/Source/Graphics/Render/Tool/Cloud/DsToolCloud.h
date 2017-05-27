#ifndef _DS_TOOL_CLOUD_H_
#define _DS_TOOL_CLOUD_H_

namespace DsLib
{
	class DsDrawCommand;
	class DsCamera;
}

namespace DsLib
{
	class DsToolCloud
	{
	public:
		DsToolCloud();
		virtual ~DsToolCloud();

	public:
		static DsToolCloud& Create(DsDrawCommand& com, const DsCamera& rendCam);

	public:
		virtual void Update( const float dt ) = NULL;
	};
}

#endif
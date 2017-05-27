#include "DsPch.h"
#include "Graphics/Render/Tool/DsRenderTool.h"
#include "Graphics/Render/Tool/Cloud/DsToolCloud.h"

using namespace DsLib;

DsRenderTool::DsRenderTool(DsDrawCommand& com, DsCamera& cam)
: m_pCloud(NULL)
{
	m_pCloud = &DsToolCloud::Create(com, cam);
}

DsRenderTool::~DsRenderTool()
{
	delete m_pCloud; m_pCloud = NULL;
}
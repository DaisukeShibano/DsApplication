#include "DsAppPch.h"
#ifndef _DS_ANIM_EVENT_TRACK_RES_
#include "Res/DsAnimEventTrackRes.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#include "Res/DsAnimEventTrackFormat.h"
#endif

using namespace DsLib;
using namespace DsApp;


DsAnimEventTrackRes::DsAnimEventTrackRes()
	: DsResItem()
	, m_pData(NULL)
{

}
DsAnimEventTrackRes::~DsAnimEventTrackRes()
{
	delete m_pData; m_pData = NULL;
}


//virtual
void DsAnimEventTrackRes::Initialize(const char* path)
{
	DsFile fs(path);
	if (fs.IsFail()) {
		return;
	}
	const ds_uint8* pBufTop = fs.MoveBufferTop();

	DS_ANIM_ET_DATA* rData = (DS_ANIM_ET_DATA*)pBufTop;
	rData->modelPathOffset += (long long)(pBufTop);
	rData->paramSetOffset += (long long)(pBufTop);

	DS_ANIM_ET_PARAM_SET* rParamSet = rData->pParamSet;
	for (int i = 0; i < rData->paramSetNum; ++i) {
		rParamSet[i].paramsOffset += (long long)(pBufTop);
		rParamSet[i].animNameOffset += (long long)(pBufTop);

		DS_ANIM_ET_PARAM* rParams = rParamSet[i].pParams;
		const char* animName = rParamSet[i].pAnimName;
		for (int pIdx = 0; pIdx < rParamSet[i].paramNum; ++pIdx) {
			rParams[pIdx].paramOffset += (long long)(pBufTop);
		}
	}
	m_pData = rData;
}





//virtual
DsResItem* DsAnimEventTrackResFactory::CreateIns() const
{
	return new DsAnimEventTrackRes();
}